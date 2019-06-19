# tom-ui

**This project is in its early stages. It's not yet reliable enough for production use.**

![](docs/screenshot%20mac.png)


## Content

**[1. Overview](#1-overview)**

**[2. Installation](#2-installation)**

* [2.1. Mac OS X](#21-mac-os-x)
* [2.2. Ubuntu Linux 18.10](#22-ubuntu-linux-1810)
* [2.3. Any platform](#23-any-platform)

**[3. Features](#3-features)**

* [3.1. Tray icon](#31-tray-icon)
* [3.2. Project tree](#32-project-tree)
* [3.3. Timeframe list](#33-timeframe-list)
* [3.4. Project quickstart window](#34-project-quickstart-window)
* [3.5. Reports](#35-reports)
* [3.6. Data import](#36-data-import)
* [3.7. Command line interface](#37-command-line-interface)

**[4. Useful resources](4-useful-resources)**

**[5. License](5-license)**

----


## 1. Overview

`tom-ui` is a time-tracking application with a graphical user interface (GUI), available for Mac OS X, Linux and Windows. It is a simple Qt5-based frontend for the [`tom`](https://github.com/jansorg/tom) command line time tracker.

`tom-ui` is being developed to not interrupt your work and to be as little hassle as possible. It will eventually support many features of the command-line software `gotime` but it's not going to be a full-featured time-tracking application.


## 2. Installation

### 2.1. Mac OS X

Download and install the latest `.dmg` files of `tom` and `tom-ui`, provided under:

* [`tom` Releases](https://github.com/jansorg/tom/releases)
* [`tom-ui` Releases](https://github.com/jansorg/tom-ui/releases)


### 2.2. Ubuntu Linux 18.10

This may also work for other versions of Ubuntu, and for other Linux distributions based on Debian.

1. Download the latest `.deb` package of `tom` provided under [`tom` Releases](https://github.com/jansorg/tom/releases).

2. Download the latest `.deb` package of `tom-ui` provided under [`tom-ui` Releases](https://github.com/jansorg/tom/releases).

3. Install the packages of both `tom`, `tom-ui` and all their dependencies with a command like this:
  
       sudo apt install ./tom_0.9.0_Linux-amd64.deb ./tom-ui_0.9.0-ubuntu-18.10_amd64.deb
       
    It is important to reference the `.deb` files with their paths (and if only `./`) as otherwise `apt` will interpret the arguments as package names and try to find them in online repositories.


### 2.3. Any platform

For any supported platform, you can always build `tom-ui` from source using `cmake`. See [DEVELOPMENT.md](https://github.com/jansorg/tom-ui/blob/master/DEVELOPMENT.md) for details.


## 3. Features

### 3.1. Tray icon

Under Linux, the tray icon will display the status: "red square" means a timer is active.

On right click, the tray icon displays the recently used projects for a quick start of a new activity.


### 3.2. Project tree

You can:

* Create, edit and delete subprojects from the context menu of a project tree entry.

* Create, edit and delete projects from the "Project" menu (this is redundant to the above, as a project is also a subproject of the "All projects" entry).

* Move projects and subprojects around by drag & drop to change the project tree hierarchy.

* Start a timer for the currently selected (sub-)project by pressing the toolbar button or selecting "Project → Start timer" from the menu.


### 3.3. Timeframe list

The lower part of the window shows the timeframes belonging to the currently selected (sub-)project. You can:

* Move a timeframe to a different (sub-)project by drag & drop into the project tree.

* Change the start or end time of a timeframe by clicking on a value of the currently selected timeframe.

* Add a note to a timeframe by clicking in the "Notes" column of the currently selected timeframe.



### 3.4. Project quickstart window

You can use the global keyboard shortcut <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd> to show the project quickstart window.

From there, you can quickly start a project timer by typing a part of a project name and choosing a suggested project.

Alternatively, you can also show the project quickstart window by choosing "Project → Lookup project …" in the menu.

Finally, if you notice you can't find the right project with the quickstart window, there is a button in its bottom right to show the main window and thus, the full project tree.


### 3.5. Reports

[TODO]


### 3.6. Data import

You can take over time-tracking data from various other applications into `tom-ui` using the "Import" menu. Choose:

* **"Mac Time Tracker CSV"** to import a CSV file exported by Max Time Tracker. [TODO: link]

* **"Fanurio CSV"** to import a CSV file exported from the time-tracking and billing software [Fanurio](https://www.fanuriotimetracking.com/).

* **"Watson frames.json"** to import the `frames.json` file of the command-line time-tracker [Watson](https://tailordev.github.io/Watson/). Under Linux, that file is typically found at `~/.config/watson/frames`.


### 3.7. Command line interface

Since `tom-ui` is just a frontend for `tom`, you can use `tom` to control your time-tracking from the command line. Also while `tom-ui` is running at the same time. For example, a typical session could be:

    tom create project acme
    tom start acme
    …
    tom stop
    tom report --month 0

For details, see the [`tom` documentation](https://github.com/jansorg/tom#readme).


## 4. Useful resources

* [FreeDesktop Icon Naming Specification](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html), a list of theme icon names that can be relied on in FreeDesktop compatible environments like KDE and Gnome

* [Configuring the content of the Mac OS X application bundle file `Info.plist` with cmake](https://cmake.org/cmake/help/latest/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html)


## 5. License

To be decided, probably GPL.

Software and assets included from third parties:

* from [Emerald Icon Theme](https://github.com/vinceliuice/emerald-icon-theme)

    * `images/icons/document-new.svg`
    * `images/icons/edit-delete.svg`
    * `images/icons/window.svg`
    * `images/icons/document-save.svg`
