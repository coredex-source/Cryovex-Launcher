#!/usr/bin/env python3
"""
Cryovex Launcher - Authentication Flow Test

This script demonstrates the Microsoft authentication flow used by the launcher.
It mirrors the C++ implementation to validate the authentication logic.
"""

import requests
import json
import urllib.parse
from typing import Dict, Optional

class MicrosoftAuthTest:
    """Test class that mirrors the C++ MicrosoftAuth implementation"""
    
    # Replace with your actual client ID from Azure App Registration
    CLIENT_ID = "YOUR_CLIENT_ID"
    REDIRECT_URI = "http://localhost:8080/auth/callback"
    
    # Microsoft/Xbox URLs (same as in C++ code)
    MICROSOFT_AUTH_URL = "https://login.microsoftonline.com/consumers/oauth2/v2.0"
    XBOX_LIVE_AUTH_URL = "https://user.auth.xboxlive.com/user/authenticate"
    XSTS_AUTH_URL = "https://xsts.auth.xboxlive.com/xsts/authorize"
    MINECRAFT_AUTH_URL = "https://api.minecraftservices.com/authentication/login_with_xbox"
    MINECRAFT_PROFILE_URL = "https://api.minecraftservices.com/minecraft/profile"

    def __init__(self):
        self.session = requests.Session()
        self.session.headers.update({
            'User-Agent': 'CryovexLauncher/1.0.0 (Test)'
        })

    def generate_auth_url(self) -> str:
        """Generate Microsoft OAuth2 authorization URL"""
        params = {
            'client_id': self.CLIENT_ID,
            'response_type': 'code',
            'redirect_uri': self.REDIRECT_URI,
            'scope': 'XboxLive.signin offline_access',
            'state': '12345'  # Should be random in production
        }
        
        url = f"{self.MICROSOFT_AUTH_URL}/authorize"
        return f"{url}?{urllib.parse.urlencode(params)}"

    def exchange_code_for_tokens(self, auth_code: str) -> Optional[Dict]:
        """Exchange authorization code for access token"""
        print(f"🔄 Exchanging authorization code for tokens...")
        
        data = {
            'client_id': self.CLIENT_ID,
            'code': auth_code,
            'grant_type': 'authorization_code',
            'redirect_uri': self.REDIRECT_URI,
            'scope': 'XboxLive.signin offline_access'
        }
        
        try:
            response = self.session.post(
                f"{self.MICROSOFT_AUTH_URL}/token",
                data=data,
                headers={'Content-Type': 'application/x-www-form-urlencoded'}
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"❌ Token exchange failed: {e}")
            return None

    def authenticate_xbox_live(self, access_token: str) -> Optional[Dict]:
        """Authenticate with Xbox Live"""
        print(f"🔄 Authenticating with Xbox Live...")
        
        payload = {
            "Properties": {
                "AuthMethod": "RPS",
                "SiteName": "user.auth.xboxlive.com",
                "RpsTicket": f"d={access_token}"
            },
            "RelyingParty": "http://auth.xboxlive.com",
            "TokenType": "JWT"
        }
        
        try:
            response = self.session.post(
                self.XBOX_LIVE_AUTH_URL,
                json=payload,
                headers={'Accept': 'application/json'}
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"❌ Xbox Live auth failed: {e}")
            return None

    def authenticate_xsts(self, xbl_token: str) -> Optional[Dict]:
        """Authenticate with XSTS"""
        print(f"🔄 Authenticating with XSTS...")
        
        payload = {
            "Properties": {
                "SandboxId": "RETAIL",
                "UserTokens": [xbl_token]
            },
            "RelyingParty": "rp://api.minecraftservices.com/",
            "TokenType": "JWT"
        }
        
        try:
            response = self.session.post(
                self.XSTS_AUTH_URL,
                json=payload,
                headers={'Accept': 'application/json'}
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"❌ XSTS auth failed: {e}")
            return None

    def authenticate_minecraft(self, xsts_token: str, user_hash: str) -> Optional[Dict]:
        """Authenticate with Minecraft"""
        print(f"🔄 Authenticating with Minecraft...")
        
        payload = {
            "identityToken": f"XBL3.0 x={user_hash};{xsts_token}"
        }
        
        try:
            response = self.session.post(
                self.MINECRAFT_AUTH_URL,
                json=payload
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"❌ Minecraft auth failed: {e}")
            return None

    def fetch_minecraft_profile(self, mc_access_token: str) -> Optional[Dict]:
        """Fetch Minecraft profile"""
        print(f"🔄 Fetching Minecraft profile...")
        
        try:
            response = self.session.get(
                self.MINECRAFT_PROFILE_URL,
                headers={'Authorization': f'Bearer {mc_access_token}'}
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            print(f"❌ Profile fetch failed: {e}")
            return None

    def complete_auth_flow(self, auth_code: str) -> Optional[Dict]:
        """Complete the entire authentication flow"""
        print("🚀 Starting Microsoft → Xbox Live → XSTS → Minecraft auth flow")
        print("=" * 60)
        
        # Step 1: Exchange code for Microsoft tokens
        token_data = self.exchange_code_for_tokens(auth_code)
        if not token_data:
            return None
        
        access_token = token_data.get('access_token')
        refresh_token = token_data.get('refresh_token')
        print(f"✅ Got Microsoft access token (expires in {token_data.get('expires_in', 'unknown')}s)")
        
        # Step 2: Xbox Live authentication
        xbl_data = self.authenticate_xbox_live(access_token)
        if not xbl_data:
            return None
        
        xbl_token = xbl_data.get('Token')
        print(f"✅ Got Xbox Live token")
        
        # Step 3: XSTS authentication
        xsts_data = self.authenticate_xsts(xbl_token)
        if not xsts_data:
            return None
        
        xsts_token = xsts_data.get('Token')
        user_hash = xsts_data.get('DisplayClaims', {}).get('xui', [{}])[0].get('uhs', '')
        print(f"✅ Got XSTS token and user hash")
        
        # Step 4: Minecraft authentication
        mc_data = self.authenticate_minecraft(xsts_token, user_hash)
        if not mc_data:
            return None
        
        mc_access_token = mc_data.get('access_token')
        print(f"✅ Got Minecraft access token")
        
        # Step 5: Fetch profile
        profile_data = self.fetch_minecraft_profile(mc_access_token)
        if not profile_data:
            return None
        
        username = profile_data.get('name')
        uuid = profile_data.get('id')
        print(f"✅ Got Minecraft profile: {username} ({uuid})")
        
        return {
            'access_token': mc_access_token,
            'refresh_token': refresh_token,
            'username': username,
            'uuid': uuid,
            'profile': profile_data
        }

def main():
    print("🎮 Cryovex Launcher - Authentication Flow Test")
    print("=" * 50)
    
    auth = MicrosoftAuthTest()
    
    if auth.CLIENT_ID == "YOUR_CLIENT_ID":
        print("❌ Please update CLIENT_ID in the script!")
        print("\n📋 Setup Instructions:")
        print("1. Go to https://portal.azure.com/#blade/Microsoft_AAD_IAM/ActiveDirectoryMenuBlade/RegisteredApps")
        print("2. Create a new application registration")
        print(f"3. Set redirect URI to: {auth.REDIRECT_URI}")
        print("4. Copy your Client ID and update the script")
        return
    
    auth_url = auth.generate_auth_url()
    print(f"🔗 Generated authentication URL:")
    print(f"   {auth_url}")
    print()
    print("📋 Manual Test Instructions:")
    print("1. Open the URL above in your browser")
    print("2. Sign in with your Microsoft account")
    print("3. Copy the 'code' parameter from the redirect URL")
    print("4. Paste it below to test the token exchange flow")
    print()
    
    # For automated testing, you would need to implement a local server
    # to catch the redirect and extract the authorization code
    auth_code = input("Enter authorization code (or 'demo' for flow demo): ")
    
    if auth_code.lower() == 'demo':
        print("\n🎭 Demo Mode - Showing authentication flow structure:")
        print("=" * 60)
        print("1. 🌐 User visits Microsoft login URL")
        print("2. 🔐 User signs in and grants permissions")
        print("3. ↩️  Microsoft redirects with authorization code")
        print("4. 🔄 Exchange code for Microsoft access token")
        print("5. 🎮 Use token to authenticate with Xbox Live")
        print("6. 🛡️  Get XSTS token for Xbox services")
        print("7. ⛏️  Exchange XSTS token for Minecraft access token")
        print("8. 👤 Fetch Minecraft profile (username, UUID)")
        print("9. 💾 Save tokens for future use")
        print("✅ Authentication complete!")
    elif auth_code:
        result = auth.complete_auth_flow(auth_code)
        if result:
            print("\n🎉 Authentication flow completed successfully!")
            print(f"   Username: {result['username']}")
            print(f"   UUID: {result['uuid']}")
            print("\n💾 In the C++ application, these tokens would be saved to:")
            print("   %APPDATA%/CryovexLauncher/auth.json")
        else:
            print("\n❌ Authentication flow failed!")
    
    print("\n🔧 This validates the authentication logic used in our C++ Qt application.")

if __name__ == "__main__":
    main()