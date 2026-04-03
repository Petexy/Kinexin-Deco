# Kinexin Deco

A KDE Plasma window decoration that renders a vertical bar with action buttons on the side of each window.

## Preview

<p align="center">
	<img src="https://i.ibb.co/60sQPM0P/deco1.png" alt="Preview 1" width="49%" />
	<img src="https://i.ibb.co/XkfhFRDR/deco2.png" alt="Preview 2" width="49%" />
</p>

## Installation

### Arch Linux (makepkg)

```bash
git clone https://github.com/petexy/Kinexin-Deco.git
cd Kinexin-Deco
makepkg -si
```

### Manual build

**Dependencies:**
- Qt6 (Core, Gui, Widgets)
- KDE Frameworks 6 (CoreAddons, Config, ConfigWidgets, KCMUtils, WidgetsAddons)
- KDecoration3
- Extra CMake Modules (ECM)
- `cmake` >= 3.19

```bash
git clone https://github.com/petexy/Kinexin-Deco.git
cd Kinexin-Deco/kinexin-deco-kwin
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
```

### Install system-wide

```bash
sudo cmake --install build
```

This installs the decoration plugin and KCM configuration module to the KDE Plasma plugin directories.

## Configuration

Open **System Settings → Window Management → Window Decorations**, select **Kinexin Bar**, then click the configure button to adjust:

- **Bar side** — Left, Right, Top, or Bottom
- **Bar width** — Width of the decoration bar (px)
- **Button size** — Diameter of action buttons (px)
- **Button spacing** — Gap between buttons (px)
- **Bar rounding** — Corner radius (px)
- **Bar color** — Background color of the bar
- **Bar opacity** — Transparency level (0–100%)
- **Title font** — Use system font or choose a custom font and size

Button order follows KDE's **Titlebar Buttons** settings (left group = top/start buttons, right group = bottom/end buttons).
