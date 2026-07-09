# Security Policy

## Supported Versions

Security fixes are targeted at the default branch.

## Reporting a Vulnerability

Please report vulnerabilities privately through GitHub Security Advisories when available, or contact the repository owner directly. Do not open public issues for exploitable bugs, leaked credentials, or sensitive operational details.

For reports, include:

- affected commit, tag, or release
- clear reproduction steps
- expected impact
- suggested mitigation, if known

## Secret Handling

Do not commit real Slack tokens. Use environment variables for `SLACK_BOT_TOKEN`, `SLACK_APP_TOKEN`, and test-only channel IDs. Rotate any token immediately if it was pushed to GitHub, even if the commit was later removed.
