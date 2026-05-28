.pragma library
.import Sailfish.Silica 1.0 as Silica

var thinBorder = Math.max(2, Math.floor(Silica.Theme.paddingSmall/3))
var cornerRadius = ('topLeftCorner' in Silica.Screen) ? Silica.Screen.topLeftCorner.radius : Silica.Theme.paddingMedium
var topNotchHeight = ('topCutout' in Silica.Screen) ? Silica.Screen.topCutout.height : 0
