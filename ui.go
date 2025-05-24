package main

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"fyne.io/fyne/v2/canvas"
)

type NeyulwToggleButton struct {
	widget.BaseWidget

	state bool

	bg     *canvas.Rectangle

	btheme *container.ThemeOverride
	button *widget.Button
	tex    *canvas.Text
	ta, tb string

	internal func()
	OnTapped func(bool)
}

func NewToggleButton(on, off string, tapped func(bool)) *NeyulwToggleButton {
	var tb *NeyulwToggleButton
	theme := fyne.CurrentApp().Settings().Theme()
	v := fyne.CurrentApp().Settings().ThemeVariant()
	var internal func()
	if on != "" {
		internal = func() {
			if tb.state {
				tb.state = false
				tb.tex.Color = theme.Color("neyulw_toggle_on", v)
				tb.bg.FillColor = theme.Color("neyulw_toggle_off", v)
				tb.tex.Text = off
			} else {
				tb.state = true
				tb.tex.Color = theme.Color("neyulw_toggle_off", v)
				tb.bg.FillColor = theme.Color("neyulw_toggle_on", v)
				tb.tex.Text = on
			}
			tb.Refresh()
		}
	} else {
		internal = func() {
			if tb.state {
				tb.state = false
				tb.tex.Color = theme.Color("neyulw_toggle_on", v)
				tb.bg.FillColor = theme.Color("neyulw_toggle_off", v)
			} else {
				tb.state = true
				tb.tex.Color = theme.Color("neyulw_toggle_off", v)
				tb.bg.FillColor = theme.Color("neyulw_toggle_on", v)
			}
			tb.Refresh()
		}
	}
	b := widget.NewButton("", func() {
		internal()
		tb.OnTapped(tb.state)
	})
	btheme := container.NewThemeOverride(b, NeyulwExtrasTheme{})
	tex := canvas.NewText(off, theme.Color("neyulw_toggle_on", v))
	tex.Alignment = fyne.TextAlignCenter
	tb = &NeyulwToggleButton{
		state: false,
		bg: canvas.NewRectangle(theme.Color("neyulw_toggle_off", v)),
		btheme: btheme,
		button: b,
		tex: tex,
		internal: internal,
		OnTapped: tapped,
		ta: on,
		tb: off,
	}
	tb.ExtendBaseWidget(tb)
	return tb
}

func (tb *NeyulwToggleButton) CreateRenderer() fyne.WidgetRenderer {
	c := container.NewStack(tb.bg, tb.btheme, tb.tex)
	return widget.NewSimpleRenderer(c)
}
