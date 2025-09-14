# Cryovex Launcher - Development Setup

## Environment Setup

### 1. Clone the Repository
```bash
git clone https://github.com/YOUR_USERNAME/cryovex-launcher.git
cd cryovex-launcher
```

### 2. Configure Environment Variables
```bash
# Copy the example environment file
cp .env.example .env

# Edit .env with your actual values
# Use your favorite text editor to fill in:
# - CRYOVEX_CLIENT_ID: Your Azure App Registration Client ID
# - CRYOVEX_REDIRECT_URI: Your redirect URI (default: http://localhost:8080/auth/callback)
```

### 3. Set Environment Variables

#### Windows (PowerShell):
```powershell
$env:CRYOVEX_CLIENT_ID="your-actual-client-id-here"
$env:CRYOVEX_REDIRECT_URI="http://localhost:8080/auth/callback"
```

#### Windows (Command Prompt):
```cmd
set CRYOVEX_CLIENT_ID=your-actual-client-id-here
set CRYOVEX_REDIRECT_URI=http://localhost:8080/auth/callback
```

#### Linux/macOS:
```bash
export CRYOVEX_CLIENT_ID="your-actual-client-id-here"
export CRYOVEX_REDIRECT_URI="http://localhost:8080/auth/callback"
```

### 4. Azure App Registration

You need to create a Microsoft Azure App Registration:

1. Go to [Azure Portal](https://portal.azure.com)
2. Navigate to "App registrations" → "New registration"
3. Configure:
   - **Name**: Cryovex Launcher
   - **Account types**: Personal Microsoft accounts only
   - **Redirect URI**: Public client/native → `http://localhost:8080/auth/callback`
4. In "Authentication" settings:
   - Enable "Allow public client flows"
   - Add Xbox Live API permissions
5. Copy the "Application (client) ID" to your `.env` file

### 5. Build and Run

#### Using Qt Creator:
1. Open `CryovexLauncher.pro` or `CMakeLists.txt`
2. Configure build kit (Qt 6.9.2 with MSVC2022 or MinGW)
3. Build and run

#### Using CMake (Command Line):
```bash
# Configure
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.9.2/msvc2022_64"

# Build
cmake --build build --config Debug

# Run
./build/CryovexLauncher.exe
```

## Security Notes

- ❌ **NEVER** commit your actual Client ID to Git
- ✅ **ALWAYS** use environment variables for sensitive data
- ✅ Keep your `.env` file local only
- ✅ Use `.env.example` to document required variables

## Troubleshooting

### "Client ID not found" error:
- Make sure environment variable is set before running the application
- Check that variable name matches exactly: `CRYOVEX_CLIENT_ID`

### Authentication fails:
- Verify Azure app registration configuration
- Check redirect URI matches exactly
- Ensure "Allow public client flows" is enabled in Azure