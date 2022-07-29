--- src/icon.c.orig	2019-02-15 23:34:26 UTC
+++ src/icon.c
@@ -62,7 +62,7 @@ static WIcon *icon_create_core(WScreen *scr, int coord
 
 static void set_dockapp_in_icon(WIcon *icon);
 static void get_rimage_icon_from_icon_win(WIcon *icon);
-static void get_rimage_icon_from_user_icon(WIcon *icon);
+static Bool get_rimage_icon_from_user_icon(WIcon *icon);
 static void get_rimage_icon_from_default_icon(WIcon *icon);
 static void get_rimage_icon_from_x11(WIcon *icon);
 
@@ -636,28 +636,31 @@ void set_icon_minipreview(WIcon *icon, RImage *image)
 void wIconUpdate(WIcon *icon)
 {
 	WWindow *wwin = NULL;
+	Bool user_icon_found;
 
 	if (icon && icon->owner)
 		wwin = icon->owner;
 
-	if (wwin && WFLAGP(wwin, always_user_icon)) {
-		/* Forced use user_icon */
-		get_rimage_icon_from_user_icon(icon);
-	} else if (icon->icon_win != None) {
-		/* Get the Pixmap from the WIcon */
-		get_rimage_icon_from_icon_win(icon);
-	} else if (wwin && wwin->net_icon_image) {
-		/* Use _NET_WM_ICON icon */
-		get_rimage_icon_from_x11(icon);
-	} else if (wwin && wwin->wm_hints && (wwin->wm_hints->flags & IconPixmapHint)) {
-		/* Get the Pixmap from the wm_hints, else, from the user */
-		unset_icon_image(icon);
-		icon->file_image = get_rimage_icon_from_wm_hints(icon);
-		if (!icon->file_image)
-			get_rimage_icon_from_user_icon(icon);
-	} else {
-		/* Get the Pixmap from the user */
-		get_rimage_icon_from_user_icon(icon);
+	user_icon_found = get_rimage_icon_from_user_icon(icon);
+
+	/* If no user icon was found, try to acquire an icon by other means. */
+	if (!user_icon_found) {
+		if (icon->icon_win != None) {
+			/* Get the Pixmap from the WIcon */
+			get_rimage_icon_from_icon_win(icon);
+		} else if (wwin && wwin->net_icon_image) {
+			/* Use _NET_WM_ICON icon */
+			get_rimage_icon_from_x11(icon);
+		} else if (wwin && wwin->wm_hints && (wwin->wm_hints->flags & IconPixmapHint)) {
+			/* Get the Pixmap from the wm_hints, else, use default */
+			unset_icon_image(icon);
+			icon->file_image = get_rimage_icon_from_wm_hints(icon);
+			if (!icon->file_image)
+				get_rimage_icon_from_default_icon(icon);
+		} else {
+			/* Get the default image */
+			get_rimage_icon_from_default_icon(icon);
+		}
 	}
 
 	update_icon_pixmap(icon);
@@ -698,12 +701,13 @@ static void get_rimage_icon_from_x11(WIcon *icon)
 	icon->file_image = RRetainImage(icon->owner->net_icon_image);
 }
 
-static void get_rimage_icon_from_user_icon(WIcon *icon)
+static Bool get_rimage_icon_from_user_icon(WIcon *icon)
 {
 	if (icon->file_image)
-		return;
+		return True;
 
 	get_rimage_icon_from_default_icon(icon);
+	return False;
 }
 
 static void get_rimage_icon_from_default_icon(WIcon *icon)
