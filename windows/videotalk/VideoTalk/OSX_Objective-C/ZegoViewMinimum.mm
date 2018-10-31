
#import <Foundation/Foundation.h>
#import <cocoa/cocoa.h>
#import "ZegoViewMinimum.h"

void showMinimizedOnMac(QDialog *dialog)
{
    NSView *view = (NSView *)dialog->winId();
    NSWindow *wnd = [view window];
    [wnd miniaturize:nil];
}
