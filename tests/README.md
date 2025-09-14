# Authentication Flow Test

This directory contains a simple Python script to test the Microsoft authentication flow 
that mirrors the implementation in the C++ code.

## Prerequisites

```bash
pip install requests
```

## Usage

1. Register your app at https://portal.azure.com/#blade/Microsoft_AAD_IAM/ActiveDirectoryMenuBlade/RegisteredApps
2. Set redirect URI to: http://localhost:8080/auth/callback  
3. Update CLIENT_ID in test_auth.py
4. Run: python test_auth.py

The script will:
- Generate the Microsoft OAuth2 URL
- Print instructions for manual testing
- Show the expected token exchange flow

This validates that our authentication logic is correct before building the full Qt application.