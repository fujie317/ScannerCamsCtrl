# ScannerCamsCtrl
This is a GUI based control program for two digital camers (not webcams) that are used in industrial document scanners. 
libgphoto2 is the primary library used for controlling various aspects of the cameras. A third party package was incorporated 
to implement a magnifying glass for zooming in and out of certain parts of the scanned images. C++ and Qt5.12 are the languages
used for the Gui part. Package was tested with two Sony Alpha 6300 digital cameras. It should work with all the cameras that 
libgphoto2 supports. 

## Build & Installation
The whole package can be imported into Qt Creator and be built from there to produce your own binary. You will need libgphoto2 
and libgphoto2_port installed locally to build. OpenGL is also required. The mesa version will be fine. Only Linux is known to 
provide the required usb drivers that allow the program to talk to the cameras. Windows seems to have problem to distinguish 
two identical cameras. Macs runnning MacOSX might work. But I have not tried it. 

## User Guide
It's quite straight forward. Start the program while the cameras (take out the memory cards) are fired up and connected via usb
 to your computer. In the ensuing window, if you don't see viable image size information in the status bar area at the bottom 
 of the screen, quit the program and restart. Some cameras need the computer to give them a "kick" to get them into the ready 
 state. 
 1. If the previews are well focused, you may start scanning, by hitting the "Scan Now" button, right away. Images are stored in 
 the same directory as your main program is in.
 2. Use the Directory menu's "Open" command to change directory to store your scanned images at any time.
 3. Use the Action menu's "Switch Cameras" command to swap the sources of the previews. 
 4. Starting page numbers can be reset by entering them in the page number showing boxes directly. Remember to hit "Enter" 
 key after entering new page numbers. If you change the smaller number, the bigger number will be changed simultaneously.
 5. Page number format can be changed thru Format menu. Arabic is the default. Roman upper and lower are the other two options.
 6. Format of output image file name can be changed dynamically thru commands under the "Format" menu. File names are made up 
 of four parts. An optional prefix can be changed dynamically. Default is empty string. "Fixed" length character padded serial 
 number. Both the length and padding character can be changed dynamically. Default is 8 and the '0' character. A dot separator 
 is fixed. After the dot is the extension, which defaults to "jpg". But again, it can be changed dynamically. 
 7. You man initiate scanning by either hit the "Scan Now" button with your mouse or hit the space bar on your keyboard once the
  button gets focus. Keyboard focus is set on this button when the program first starts. You may repeatedly hitting the tab key 
  to move the focus or by hitting the "Scan Now" button (will effect one scan, though). Focus will remain until changed manually.
  8. You can right click at any point in the two preview areas to bring a magnifying glass window. You may use mouse wheel to 
  change the zoom level. If you want to move the glass, just right click at the new location. Dragging it does not work well 
  as the previews are constantly changing images (a.k.a video!). 
