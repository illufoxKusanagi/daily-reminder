# GitHub Actions Workflows

This directory contains automated build workflows for Daily Reminder.

## Workflows

### 1. `build-windows.yml` - Windows Build
**Triggers:**
- Push to `master`/`main` branch
- Pull requests
- Manual trigger (workflow_dispatch)
- Tags starting with `v*`

**What it does:**
- Builds Windows executable with Qt 6
- Bundles all dependencies with windeployqt
- Creates ZIP package
- Uploads as artifact (available for 30 days)
- Creates GitHub Release if triggered by a tag

**Output:** `DailyReminder-Windows-x64.zip`

### 2. `build-linux.yml` - Linux RPM Build
**Triggers:**
- Push to `master`/`main` branch
- Pull requests
- Manual trigger (workflow_dispatch)
- Tags starting with `v*`

**What it does:**
- Builds on Fedora container
- Creates RPM package
- Uploads as artifact (available for 30 days)
- Creates GitHub Release if triggered by a tag

**Output:** `daily-reminder-1.0.0-*.rpm`

### 3. `release.yml` - Complete Release (Both Platforms)
**Triggers:**
- Tags matching `v*.*.*` (e.g., v1.0.0, v1.2.3)
- Manual trigger with version input

**What it does:**
- Creates GitHub Release
- Builds Windows executable
- Builds Linux RPM
- Uploads both to the release

**Outputs:**
- `DailyReminder-Windows-x64.zip`
- `daily-reminder-1.0.0-linux-x86_64.rpm`

## How to Use

### For Development (Automatic Builds)

Just push to `master`:
```bash
git add .
git commit -m "Your changes"
git push
```

The workflows will automatically:
1. Build Windows and Linux versions
2. Upload artifacts to GitHub Actions
3. Artifacts are available in the "Actions" tab for 30 days

### For Releases

**Option 1: Using Git Tags**
```bash
# Create and push a tag
git tag v1.0.0
git push origin v1.0.0
```

**Option 2: Manual Release**
1. Go to GitHub → Actions tab
2. Select "Create Release" workflow
3. Click "Run workflow"
4. Enter version (e.g., v1.0.0)
5. Click "Run workflow"

Both methods will:
1. Create a GitHub Release
2. Build for Windows and Linux
3. Upload binaries to the release
4. Users can download from Releases page

## Download Artifacts

### From Actions Tab (Development Builds)
1. Go to repository → Actions tab
2. Click on a workflow run
3. Scroll to "Artifacts" section
4. Download `DailyReminder-Windows-x64` or `DailyReminder-Linux-RPM`

### From Releases Page (Tagged Releases)
1. Go to repository → Releases tab
2. Click on a release
3. Download from "Assets" section

## Build Status

Check build status:
- ✅ Green checkmark = Build successful
- ❌ Red X = Build failed
- 🟡 Yellow dot = Build in progress

## Workflow Files Structure

```
.github/workflows/
├── build-windows.yml    # Windows builds on every push
├── build-linux.yml      # Linux RPM builds on every push  
└── release.yml          # Complete releases with tags
```

## Requirements

These workflows use:
- **Qt**: Installed via jurplel/install-qt-action
- **Node.js**: Version 18
- **CMake**: For building backend
- **MinGW**: For Windows compilation
- **Fedora container**: For Linux RPM builds

All dependencies are automatically installed by the workflows.

## Secrets Required

None! All workflows use `GITHUB_TOKEN` which is automatically provided by GitHub Actions.

## Tips

1. **Test before release**: Push to `master` first to test builds
2. **Use semantic versioning**: v1.0.0, v1.1.0, v2.0.0, etc.
3. **Check Actions tab**: Monitor build progress and logs
4. **Download artifacts**: Test the builds before creating official releases
5. **Update version in code**: Remember to update version numbers in your code

## Troubleshooting

**Build fails on Windows:**
- Check Qt installation step
- Verify CMake configuration
- Check windeployqt output

**Build fails on Linux:**
- Check Fedora dependencies
- Verify Qt6 packages are available
- Check RPM spec file

**Release not created:**
- Ensure tag format is correct (v1.0.0)
- Check GITHUB_TOKEN permissions
- Verify release.yml syntax

## Local Testing

To test locally before pushing:

**Windows:**
```bash
# Follow the same steps as the workflow
npm install && npm run build
cd backend && mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
windeployqt.exe backend.exe
```

**Linux:**
```bash
./build-rpm-new.sh
```
