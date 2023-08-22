# How to distribute your application
So often in the software community, I come across cool people who make neat projects that are ultimately only used by one person on one computer. 
This is a walkthrough of my process of getting out of that trap with a recent application I wrote.
## The problem
I use [Arch linux](https://archlinux.org) and have a customized [sway](https://swaywm.org) desktop. 
One of the very common things people do with their desktops, especially in the linux space is to use a transparent terminal. 
It improves the aesthetics of an otherwise usually un-pleasing interface.

I realized that I saw a whole lot more of my desktop background than I'm used to, and was constantly running `swaymsg set-desktop $SOME_IMAGE` to change it. 
Why not, I thought to myself, have a keyboard macro that switches my desktop background? 
So I spent a little time writing one and used it for years until I went to primarily using an apple computer. 
I re-wrote the same script and re-bound the same key macro and was satisfied. 
It wasn't until [a friend of mine](https://github.com/DeaSTL) was setting his linux desktop up that I thought about it again. 
I sent him my script, and realized a whole lot of it had to be changed to get it to work on his machine. 
He decided that maintaining my shell script would be too much hassle and decided to write a python script himself. 

The gauntlet had been thrown.
If my friends (or enemies or anyone else) wants to do this, I want it to be easy. 
My new goal was to make a fully polished, release ready, wallpaper changing application for everyone

## Initial Work
The first thing I did was choose a language. Shell and python were out because they can't reliably found on Windows computers. 
I wanted a compiled language to avoid entering dependency hell on another person's machine. 
I needed something reliable and relatively fast so the main bottle-neck would be the speed of changing a wallpaper, and not running the program. 
I decided to go with [go](https://golang.org)
I went ahead and translated my shell script to go.

### The basics
The program is relatively simple at this point. 
You call it with no arguments, and it does the following:
- Reads a state file containing nothing but the path to the current wallpaper set. 
- Looks through the directory containing it, and picks a new wallpaper
- Sets the system wallpaper to the wallpaper it chose

Additionally, if you call it with a `-c` argument, it does the following:
- Reads the same state file
- looks through the directory containing the directory containing the wallpaper
- presents a list of directories to the user via a [rofi](https://github.com/davatorium/rofi) (or rofi-like) command.
- changes the wallpaper to a random one from the newly-selected directory

It is relatively easy, then, to bind a call to my application to change the wallpaper

### Mac
Getting it running on macOS is relatively easy. I just use [conditional compilation](https://stackoverflow.com/questions/38950909/c-style-conditional-compilation-in-golang) to make the wallpaper directory chooser and wallpaper setter subroutines work on mac.
I figure that it's probably fine to make the [choose](https://github.com/geier/choose) utility a required dependency. 
With those changes made, I figure it is be easy enough to include instructions on how to make a key-binding to call the application. 

### Windows
Moving the program to Windows is the first source of real complexity. 
The filepaths were all different, using `\` instead of `/` for separating directories, and requiring a drive identifier to signify the root instad of a single `/`. 
This exposes the sloppiness I've introduced in parsing and handling filepaths.
This will be fixed eventually, but for now, I've put in a simple switch statement which allows for equally sloppy path parsing on Windows. 
As for the actual setting of the wallpaper, I luckily found [this blogpost](https://anubissec.github.io/How-To-Call-Windows-APIs-In-Golang/#) which basically told me what to do.
The choosing of the wallpaper directory, however, was a different story.
I looked and couldn't find a suitable replacement for rofi or choose in the windows world, so I had to make a UI.
Luckily, I have used the [fyne toolkit](https://fyne.io) before with great success and was able to whip something up that shows a list of directories and returns the selction back to the program.
I went back to mac and linux and tested the chooser to great success. 

## Packaging
Now that I have a program that more-or-less does exactly what I want it to, I need to make it easy to install and use. 
For Linux users, they will generally be satisfied with running a few shell commands and moving a binary around to run the program so I'm not worried about them. 
The people I'm worried about are Mac and Windows users. 

### Mac
Apple programs are generally distributed as [`.app` bundles](https://en.wikipedia.org/wiki/Bundle_(macOS)).
These are directories containing the following things (and sometimes more):
- The application binary
- A launcher for the application binary
- An `Info.Plist` file
- Assets to be used within the application

Between `cd`-ing into a few existing app bundles, copying the code from the last time I did this, and googling, I eventually came up with a template for an app bundle to be built from a Makefile.
The convention for Mac application distribution is usually to make a `.dmg` archive file that include a link to the applications directory with installation being as simple as opening the archive, and moving the app bundle into the applications folder. 
I included a call to the [create-dmg](https://github.com/create-dmg/create-dmg) program in the Makefile to handle generating this file.

### Windows
Windows installer files are much more complicated than Mac. 
There are a handful of common ways to package and distribute applications on Windows. 
The most popular are as follows
- an exe installer file
- a zip file containing the program executable and dependencies
- an msi installer file

I am going with an msi file because I cheated off of the work I did for a previous project, that I was able to do by cheating off of the [alacritty](https://github.com/alacritty/alacritty) program.
This process is quite in-depth even with the fantastic [wix toolset](https://wixtoolset.org/), and should be covered in it's own blogpost.
Just trust me when I say that coming from a linux-centric background, it's much more complicated than it has any right to be.

## UI
The program is now ready to distribute, but it's annoying to set up keybindings in Windows, and I wouldn't want to subject my Windows users to executing manual steps just to do something as "simple" as changing their wallpaper. 
At this point, after installing the program, it shows up in the start menu and can be run, which does change the wallpaper, and it can be put in the launcher bar to save a click. The only problem is that changing which directory it's pulling wallpapers from is totally inaccessible. 

I noticed that on the firefox launcher icon, I can write click and get a special menu that will allow me to open a new tab or a private window.
I figure that this can't be so hard (I mean come on, the people over at mozilla could pull it off) and I went ahead and embarked on the process of adding a similar menu.
After a little research, I discover that the menu is called a "Jump menu", and that It can be generated by calling [this code](https://learn.microsoft.com/en-us/uwp/api/windows.ui.startscreen.jumplist?view=winrt-22621) at some point. 
There is only one problem with calling that code. The only examples are in C# and I don't know how to write C#, and I definitely don't know how to write .net C#.

### Descent into madness
I spend the next 2 days digging into every piece of documentation there is on running c code from go, running c++ code from c, exporting c# functions to c++, other apis for doing the same task, windows program standards, and other things. Some of it works for basic things, but I come up woefully short getting it to work properly. AHA! I think, I'll just do it on apple (because certainly it's easier with xcode) and dig into the apple documentation on the same feature. It turns out that it's even harder.
If I was a better programmer, I'd power through and figure it out, but I'm not that smart (I write go all day. How smart do I need to be?).
