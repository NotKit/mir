#!/usr/bin/env python3
# coding: utf-8

import json
import logging
import os
import re

from launchpadlib.credentials import (RequestTokenAuthorizationEngine,
                                      UnencryptedFileCredentialStore)
from launchpadlib.launchpad import Launchpad
import requests


logger = logging.getLogger("mir.process_snaps")
logger.addHandler(logging.StreamHandler())
logger.setLevel(logging.INFO)

APPLICATION = "mir-ci"
LAUNCHPAD = "production"
RELEASE = "xenial"
TEAM = "mir-team"
SNAP = "mir-kiosk"
SOURCE_NAME = "mir"
CHANNELS = {
    "edge": ("dev", "mir-kiosk-edge"),
    "candidate": ("rc", "mir-kiosk-candidate"),
}


PENDING_BUILD = (
    "Needs building",
    "Dependency wait",
    "Currently building",
    "Uploading build",
)

VERSION_RE = re.compile(r"^(.*)-[^-]+$")

STORE_URL = ("https://api.snapcraft.io/api/v1/snaps"
             "/details/{snap}?channel={channel}")
STORE_HEADERS = {
    "X-Ubuntu-Series": "16",
    "X-Ubuntu-Architecture": "{arch}"
}


def check_store_version(processor):
    logger.debug("Checking version for: %s", processor)
    data = {
        "snap": SNAP,
        "channel": channel,
        "arch": processor,
    }
    resp = requests.get(
        STORE_URL.format(**data),
        headers={k: v.format(**data) for k, v in STORE_HEADERS.items()}
    )
    logger.debug("Got store response: %s", resp)

    try:
        result = json.loads(resp.content)
    except json.JSONDecodeError:
        logger.error("Could not parse store response: %s",
                     resp.content)
        return

    try:
        return result["version"]
    except KeyError:
        logger.debug("Could not find version for %s (%s): %s",
                     SNAP, processor, result["error_list"])
    return None


if __name__ == '__main__':
    try:
        lp = Launchpad.login_with(
            APPLICATION,
            LAUNCHPAD,
            version="devel",
            authorization_engine=RequestTokenAuthorizationEngine(LAUNCHPAD,
                                                                 APPLICATION),
            credential_store=UnencryptedFileCredentialStore(
                os.path.expanduser("~/.launchpadlib/credentials"))
        )
    except NotImplementedError:
        raise RuntimeError("Invalid credentials.")

    ubuntu = lp.distributions["ubuntu"]
    logger.debug("Got ubuntu: %s", ubuntu)

    series = ubuntu.getSeries(name_or_version=RELEASE)
    logger.debug("Got series: %s", series)

    team = lp.people[TEAM]
    logger.debug("Got team: %s", team)

    for channel in CHANNELS:
        logger.info("Processing channel %s…", channel)

        ppa = team.getPPAByName(name=CHANNELS[channel][0])
        logger.debug("Got ppa: %s", ppa)

        snap = lp.snaps.getByName(owner=team, name=CHANNELS[channel][1])
        logger.debug("Got snap: %s", snap)

        latest_source = ppa.getPublishedSources(
            source_name=SOURCE_NAME,
            distro_series=series
        )[0]
        logger.debug("Latest source: %s", latest_source.display_name)

        version = VERSION_RE.match(latest_source.source_package_version)[1]
        logger.debug("Parsed upstream version: %s", version)

        store_versions = {
            processor.name: check_store_version(processor.name)
            for processor in snap.processors
        }

        logger.debug("Got store versions: %s", store_versions)

        if all(store_version == version
               for store_version in store_versions.values()):
            logger.info("Skipping %s: store versions are current",
                        latest_source.display_name)
            continue

        if latest_source.status != "Published":
            logger.info("Skipping %s: %s",
                        latest_source.display_name, latest_source.status)
            continue

        if any(build.buildstate in PENDING_BUILD
               for build in latest_source.getBuilds()):
            logger.info("Skipping %s: builds pending…",
                        latest_source.display_name)
            continue

        if any(binary.status != "Published"
               for binary in latest_source.getPublishedBinaries()):
            logger.info("Skipping %s: binaries pending…",
                        latest_source.display_name)
            continue

        if len(snap.pending_builds) > 0:
            logger.info("Skipping %s: snap builds pending…", snap.web_link)
            continue

        logger.info("Triggering for %s…", latest_source.display_name)

        builds = snap.requestAutoBuilds()
        logger.debug("Triggered builds: %s", snap.web_link)
