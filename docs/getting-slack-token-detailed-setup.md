# Getting Slack Token - Detailed Setup Guide

# Complete Guide: How to Get Slack API Token

## Table of Contents

1. [What is Slack App](#what-is-slack-app)
2. [Key Concepts](#key-concepts)
3. [Step-by-Step Instructions](#step-by-step-instructions)
4. [What You Need and What You Don't](#what-you-need-and-what-you-dont)
5. [Token Verification and Usage](#token-verification-and-usage)
6. [Token Security](#token-security)
7. [Troubleshooting](#troubleshooting)
8. [Additional Resources](#additional-resources)

## What is Slack App?

**Slack App** is an application that integrates with Slack and can:
- Receive events (messages, mentions, commands)
- Send messages
- Interact with users
- Perform various actions in Slack workspace

**For Neo Slack Bot**, you need a Slack App to:
- Connect to Slack via Socket Mode (WebSocket)
- Receive messages in real-time
- Send responses via HTTP API
- Handle commands and mentions

## Key Concepts

### Socket Mode

**Socket Mode** is a way to connect to Slack via WebSocket connection.

**How it works:**
- Your application establishes a persistent WebSocket connection with Slack
- Slack sends events (messages, mentions) through this connection in real-time
- No need to configure a public URL or server
- Perfect for local applications and bots

**For Neo Slack Bot:** ✅ **REQUIRED** — this is the primary way to receive events

**Where to find it:** In app settings → **"Socket Mode"** (left sidebar)

### Event Subscriptions

**Event Subscriptions** is a way to receive events via HTTP webhooks.

**How it works:**
- Slack sends HTTP POST requests to your public URL when events occur
- Requires a public server with HTTPS
- Need to configure URL and verify signatures

**For Neo Slack Bot:** ❌ **NOT NEEDED** — we use Socket Mode instead

### Bot Token (Bot User OAuth Token)

**Bot Token** is a token for authenticating your bot.

**Format:** `xoxb-[numbers]-[alphanumeric]`
**Example:**
```
xoxb-[your-workspace-id]-[your-bot-id]-[your-token-secret]
```

**What it does:**
- Used for connecting via Socket Mode
- Used for HTTP API requests (sending messages)
- Provides access with bot permissions

**For Neo Slack Bot:** ✅ **REQUIRED** — this is the primary token

**Where to find it:** In app settings → **"OAuth & Permissions"** → after installing to workspace

### App-Level Token

**App-Level Token** is a token at the application level (not user level).

**Format:** `xapp-1-[alphanumeric]`
**Example:**
```
xapp-1-[your-app-id]-[your-token-secret]
```

**What it does:**
- Used for Socket Mode v2 connection via `apps.connections.open` API
- Required for getting dynamic WebSocket URL
- Provides `connections:write` scope for Socket Mode
- Doesn't require workspace installation

**For Neo Slack Bot:** ✅ **REQUIRED** — Neo Slack Bot uses Socket Mode v2, which requires App-Level Token to get dynamic WebSocket URL. Without it, the application will fail to connect with `403 Forbidden` error.

**Where to find it:** In app settings → **"Socket Mode"** → **"App-Level Tokens"**

**⚠️ IMPORTANT:** Without `SLACK_APP_TOKEN`, the application will not be able to connect to Slack and will show `403 Forbidden` errors. Make sure to set this token before running the bot.

### OAuth Scopes (Permissions)

**OAuth Scopes** are permissions that your application requests.

**What this means:**
- Each scope provides specific capabilities
- For example: `chat:write` allows sending messages
- User must approve these permissions during installation

**For Neo Slack Bot:** ✅ **REQUIRED** — need to configure correct scopes

**Where to find it:** In app settings → **"OAuth & Permissions"** → **"Bot Token Scopes"**

### Install to Workspace

**Install to Workspace** is the process of adding the bot to your Slack workspace.

**What happens:**
- User (you) approves access permissions
- Slack generates Bot Token for your workspace
- Bot appears in workspace and can operate

**For Neo Slack Bot:** ✅ **REQUIRED** — without this, Bot Token won't be created

**Where to find it:** In app settings → **"OAuth & Permissions"** → **"Install to Workspace"** button

## Step-by-Step Instructions

### Step 1: Create Slack App

#### 1.1. Open App Creation Page

1. Go to **https://api.slack.com/apps**
2. If you're not logged in, sign in to your Slack account
3. You'll see a list of your apps (if any)

#### 1.2. Create New App

1. Click the **"Create New App"** button (usually in the top right corner or center)
2. A modal window will appear with two options:
   - **"From scratch"** — create from scratch
   - **"From an app manifest"** — create from manifest
3. Select **"From scratch"**
4. Click **"Next"**

#### 1.3. Fill in App Information

1. In the **"App Name"** field, enter a name, e.g.: `Neo Slack Bot`
2. In the **"Pick a workspace to develop your app in"** field, select your workspace from the dropdown
3. Click the **"Create App"** button

#### 1.4. You're Now on the App Settings Page

You'll see:
- Left sidebar with settings sections
- Central area with settings
- App information on the right

**Important:** Remember or bookmark this page — you'll return here for configuration.

### Step 2: Enable Socket Mode

#### 2.1. Find Socket Mode Section

1. In the **left sidebar**, find the **"Socket Mode"** section
2. Click on it
3. You'll be taken to the Socket Mode settings page

#### 2.2. Enable Socket Mode

1. You'll see a toggle switch **"Enable Socket Mode"**
2. Toggle it to **ON** (enabled)
3. A warning will appear that Socket Mode is enabled

**Important:** After enabling Socket Mode, Event Subscriptions are automatically disabled (this is normal, we don't need them).

#### 2.3. Create App-Level Token (REQUIRED)

**⚠️ IMPORTANT:** For Neo Slack Bot, App-Level Token is **REQUIRED**. The application uses Socket Mode v2, which requires App-Level Token to get dynamic WebSocket URL via `apps.connections.open` API.

1. Scroll down to the **"App-Level Tokens"** section
2. Click the **"Generate Token"** button
3. In the **"Token Name"** field, enter: `socket-mode-token` (or any name you prefer)
4. In the **"Scopes"** section, add scope: `connections:write`
5. Click **"Generate"**
6. **IMPORTANT:** Copy the token (starts with `xapp-`) — it's shown only once!
7. Save the token in a secure place

**Token format:** `xapp-1-A1234567890-1234567890123-abcdefghijklmnopqrstuvwxyz...`

**⚠️ Without this token, the application will fail to connect and show `403 Forbidden` errors.**

### Step 3: Configure Bot Token Scopes (Permissions)

#### 3.1. Open OAuth & Permissions Section

1. In the **left sidebar**, find the **"OAuth & Permissions"** section
2. Click on it
3. You'll be taken to the OAuth settings page

#### 3.2. Find Bot Token Scopes Section

1. Scroll down the page
2. Find the **"Scopes"** section
3. Under it, find the **"Bot Token Scopes"** subsection
4. You'll see a list of already added scopes (if any) and an **"Add an OAuth Scope"** button

#### 3.3. Add Required Scopes

Click the **"Add an OAuth Scope"** button and add the following scopes one by one:

**Required scopes:**

1. **`app_mentions:read`**
   - What it does: Allows receiving events when bot is mentioned (@bot)
   - Where to find: Type "app_mentions" in search

2. **`channels:read`**
   - What it does: Allows reading channel information
   - Where to find: Type "channels:read" in search

3. **`channels:history`**
   - What it does: Allows reading message history in channels
   - Where to find: Type "channels:history" in search

4. **`chat:write`**
   - What it does: Allows sending messages on behalf of bot
   - Where to find: Type "chat:write" in search

5. **`im:read`**
   - What it does: Allows reading direct messages (DM)
   - Where to find: Type "im:read" in search

6. **`im:write`**
   - What it does: Allows sending direct messages (DM)
   - Where to find: Type "im:write" in search

7. **`users:read`**
   - What it does: Allows reading user information
   - Where to find: Type "users:read" in search

**Optional scopes (can be added later):**

- **`commands`** — for handling slash commands (if you plan to use them)
- **`reactions:read`** — for reading reactions
- **`reactions:write`** — for adding reactions

#### 3.4. Verify Scope List

After adding all scopes, you should see a list of 7+ scopes in the "Bot Token Scopes" section.

**Important:** Scopes are added immediately, but to apply them, you need to reinstall the app in workspace (see next step).

### Step 4: Install App to Workspace

#### 4.1. Find OAuth Tokens for Your Workspace Section

1. Stay on the **"OAuth & Permissions"** page
2. Scroll **up** the page
3. Find the **"OAuth Tokens for Your Workspace"** section
4. You'll see token information and an **"Install to Workspace"** button

#### 4.2. Click Install to Workspace

1. Click the **"Install to Workspace"** button
2. A new page will open with permission request

#### 4.3. Review and Approve Permissions

1. You'll see a list of all scopes you added
2. Read what permissions the app is requesting
3. If everything is fine, click the **"Allow"** button
4. If something doesn't suit you, click **"Cancel"** and return to scope configuration

#### 4.4. After Approval

1. You'll be redirected back to the **"OAuth & Permissions"** page
2. In the **"OAuth Tokens for Your Workspace"** section, a token will now appear
3. Find **"Bot User OAuth Token"**
4. You'll see a token starting with `xoxb-`
5. Next to the token, there will be a **"Copy"** button or copy icon

#### 4.5. Copy Bot Token

1. Click the **"Copy"** button next to Bot User OAuth Token
2. **IMPORTANT:** The token is shown in full only here
3. Save the token in a secure place (e.g., password manager)


## See Also

- [Getting Slack Token - Detailed Usage](getting-slack-token-detailed-usage.md) - Token verification, security, troubleshooting
- [Getting Slack Token - Quick Guide](getting-slack-token-quick.md) - Quick reference
