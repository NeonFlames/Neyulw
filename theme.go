package main

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/theme"

	"image/color"
)

type NeyulwTheme struct {}
type NeyulwExtrasTheme struct {}
var _ fyne.Theme = (*NeyulwTheme)(nil)
var _ fyne.Theme = (*NeyulwExtrasTheme)(nil)

func neyulwThemeColor(name fyne.ThemeColorName, variant fyne.ThemeVariant) color.Color {
	switch name {
	case theme.ColorNameForeground:
		return color.RGBA{0,255,0,255}
	case theme.ColorNameBackground:
		return color.RGBA{0,0,0,255}
	case "neyulw_toggle_on":
		return color.RGBA{0,255,0,255}
	case "neyulw_toggle_off":
		return color.RGBA{20,20,20,255}
	default:
		return theme.DefaultTheme().Color(name, variant)
	}
}

func (nT NeyulwTheme) Color(name fyne.ThemeColorName, variant fyne.ThemeVariant) color.Color {
	return neyulwThemeColor(name, variant)
}

func (nT NeyulwTheme) Icon(name fyne.ThemeIconName) fyne.Resource {
	return theme.DefaultTheme().Icon(name)
}

func (nT NeyulwTheme) Font(style fyne.TextStyle) fyne.Resource {
	return theme.DefaultTheme().Font(style)
}

func (nT NeyulwTheme) Size(name fyne.ThemeSizeName) float32 {
	return theme.DefaultTheme().Size(name)
}

func (nET NeyulwExtrasTheme) Color(name fyne.ThemeColorName, variant fyne.ThemeVariant) color.Color {
	switch (name) {
	case theme.ColorNameButton:
		return color.RGBA{0,0,0,0}
	default:
		return neyulwThemeColor(name, variant)
	}
}

func (nET NeyulwExtrasTheme) Icon(name fyne.ThemeIconName) fyne.Resource {
	return theme.DefaultTheme().Icon(name)
}

func (nET NeyulwExtrasTheme) Font(style fyne.TextStyle) fyne.Resource {
	return theme.DefaultTheme().Font(style)
}

func (nET NeyulwExtrasTheme) Size(name fyne.ThemeSizeName) float32 {
	return theme.DefaultTheme().Size(name)
}

