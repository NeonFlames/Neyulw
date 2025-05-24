package main

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"fyne.io/fyne/v2/layout"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/dialog"

	"github.com/goccy/go-yaml"
	"github.com/go-ini/ini"

	"embed"
	"os"
	"fmt"
	"strings"
	"sync"
	"runtime"
	"image/color"
	"path/filepath"
)

//go:embed assets/configs
var embedded_configs embed.FS
var configs map[string]Config

type Config struct {
	Name string
	Vars map[string]struct {
		Name string
		Type string
		Hint string `yaml:",omitempty"`
	}
	Targets map[string]string
	Presets map[string]struct {
		Hint string `yaml:",omitempty"`
		Id int `yaml:",omitempty"`
		Options map[string]string
		Subsets map[string]struct {
			Hint string `yaml:",omitempty"`
			Options map[string]string 
		} `yaml:",omitempty"`
	}
	Configs map[string]map[string]struct {
		Name string `yaml:",omitempty"`
		Options map[string]struct {
			Hint string `yaml:",omitempty"`
		}
	}
}

type UserConfig struct {
	Parent string
	Options map[string]Option
}

type Option struct {
	File, Section, Value string
}

type OptionInfo struct {
	File, Section, Hint string
}

func ParseConfig(config_path string) interface{} {
	data, err := os.ReadFile(config_path)
	if err != nil { 
		return nil
	}
	return ParseConfigData(data)
}

func ParseConfigData(config []byte) Config {
	var conf Config
	if err := yaml.Unmarshal(config, &conf); err != nil {
		panic(err) // Maybe replace with nil
	} else {
		return conf
	}
}

func ParseUserConfig(config_path string) interface{} {
	data, err := os.ReadFile(config_path)
	var config UserConfig
	if err != nil || yaml.Unmarshal(data, &config) != nil  {
		return nil
	}
	return config
}

/*func RConfig(config Config, option string) *Option {
	for file, sections := range(config.Configs) {
		for section_id, section := range(sections) {
			if _, ok := section.Options[option]; ok {
				return &Option{
					file,
					section_id,
					"",
				}
			}
		}
	}
	return nil
}*/

func setup() {
	os.Setenv("FYNE_THEME", "dark")
	embedded, err := embedded_configs.ReadDir("assets/configs")
	if err != nil {
		panic(err)
	}

	data := make(map[string]Config)
	for _, element := range(embedded) {
		file_data, err := embedded_configs.ReadFile(fmt.Sprintf("assets/configs/%s", element.Name()))
		if err != nil {
			panic(err)
		}
		config_data := ParseConfigData(file_data)
		data[config_data.Name] = config_data
	}
	configs = data
	
	// TODO: Config inheritance
}

func configureGame(app fyne.App, conf Config, user_conf interface{}, vars map[string]string) fyne.Window {
	window := app.NewWindow("Neyulw")
	content := make([]*fyne.Container, 4)
	
	var option_map_mutex sync.Mutex
	option_map := make(map[string]OptionInfo)
	for file, sections := range(conf.Configs) {
		var file_group sync.WaitGroup
		for section_id, section := range(sections) {
			for option, info := range(section.Options) {
				file_group.Add(1)
				go func() {
					option_map_mutex.Lock()
					option_map[option] = OptionInfo{file, section_id, info.Hint}
					file_group.Done();
					option_map_mutex.Unlock()
				}()
			}
		}
		file_group.Wait()
	}

	var uconf UserConfig
	if user_conf != nil {
		uconf = user_conf.(UserConfig)
	} else {
		uconf = UserConfig{
			Parent: "",
			Options: make(map[string]Option),
		}
	}
	user_options := make(map[string]struct{object fyne.CanvasObject; le *widget.Entry})
	var add_user_option func(option, file, section, value string)
	var remove_user_option func(option string)
	add_user_option_ := func(option, file, section, value string) {
		uconf.Options[option] = Option{file, section, value}
		le := widget.NewEntry()
		le.Text = value
		le.OnChanged = func(text string) {
			v := uconf.Options[option]
			v.Value = text
			uconf.Options[option] = v
		}
		l := widget.NewLabel(option)
		l.Alignment = fyne.TextAlignCenter
		l.Truncation = fyne.TextTruncateEllipsis
		bg := canvas.NewRectangle(color.RGBA{50,50,50,255})
		if len(user_options) %2 != 0 {
			bg.FillColor = color.RGBA{80,80,80,255}
		}
		c := container.NewStack(bg, container.NewVBox(l, le))
		user_options[option] = struct{object fyne.CanvasObject; le *widget.Entry}{c, le}
	}
	remove_user_option_ := func(option string) {
		delete(uconf.Options, option)
		delete(user_options, option)
	}

	var search *widget.Entry
	search_only := 0
	search1 := func(text string, enabled, hints bool) {}
	search2 := func(text string, enabled, hints bool) {}
	
	{
		logo := widget.NewLabel("Neyulw")
		logo.Alignment = fyne.TextAlignCenter
		logo.Wrapping = fyne.TextWrapBreak
		presets := widget.NewButton("Presets", func() {
			presets_a := make(map[string]int, len(conf.Presets))
			presets_b := make(map[int]string, len(conf.Presets))
			presets_c := make(map[string]*fyne.Container, len(conf.Presets))
			presets_d := make(map[string]string, len(conf.Presets))
			add_order := make([]*fyne.Container, len(conf.Presets))
			disabled := container.NewVBox()
			enabled := container.NewVBox()
			refresh_enabled := func() {
				enabled.RemoveAll()
				index := 0
				for index < len(presets_b) {
					if item, ok := presets_c[presets_b[index]]; ok {
						enabled.Add(item)
					}
					index++
				}
				enabled.Refresh()
			}
			refresh_containers := func() {
				diff := 0
				i := 0
				for i < len(conf.Presets) {
					if preset, ok := presets_b[i]; ok {
						if diff != 0 {
							presets_b[i-diff] = preset
							presets_a[preset] = i-diff
							delete(presets_b, i)
						}
					} else {
						diff++
					}
					i++
				}
			}
			for preset_id, preset := range(conf.Presets) {
				p_id := widget.NewLabel(preset_id)
				p_id.Truncation = fyne.TextTruncateEllipsis
				p_id.Alignment = fyne.TextAlignCenter
				p := container.NewVBox(p_id)
				p_enabled := container.NewStack(p)
				p_disabled := container.NewStack(p)

				if preset.Hint != "" {
					p_hint := widget.NewLabel(preset.Hint)
					p_hint.Alignment = fyne.TextAlignCenter
					p_hint.Wrapping = fyne.TextWrapBreak
					p.Add(p_hint)
				}
				
				var p_enable *widget.Button
				var p_enabled_controls *fyne.Container
				p_enable = widget.NewButtonWithIcon("", theme.NavigateNextIcon(), func() {
					p_disabled.Hide()
					p_enable.Hide()
					p_enabled_controls.Show()
					p_enabled.Show()
					priority := len(presets_a)
					presets_a[preset_id] = priority
					presets_b[priority] = preset_id
					refresh_enabled()
				})
				p_disable := widget.NewButtonWithIcon("", theme.NavigateBackIcon(), func() {
					delete(presets_b, presets_a[preset_id])
					delete(presets_a, preset_id)
					refresh_containers()
					p_enabled.Hide()
					p_enabled_controls.Hide()
					p_enable.Show()
					p_disabled.Show()
					refresh_enabled()
				})
				p_prioritize := widget.NewButtonWithIcon("", theme.MoveUpIcon(), func() {
					current := presets_a[preset_id]
					if current == 0 {
						return
					}
					replace := presets_b[current-1]
					presets_a[preset_id] = current-1
					presets_a[replace] = current
					presets_b[current] = replace
					presets_b[current-1] = preset_id
					refresh_enabled()
				})
				p_deprioritize := widget.NewButtonWithIcon("", theme.MoveDownIcon(), func() {
					current := presets_a[preset_id]
					if current == len(presets_a) {
						return
					}
					replace := presets_b[current+1]
					presets_a[preset_id] = current+1
					presets_a[replace] = current
					presets_b[current] = replace
					presets_b[current+1] = preset_id
					refresh_enabled()
				})
				p_enabled_controls = container.NewGridWithColumns(3, p_disable, p_prioritize, p_deprioritize)
				p_enabled_controls.Hide()

				controls := container.NewStack(p_enable, p_enabled_controls)

				if preset.Subsets != nil {
					subsets := make([]string, len(preset.Subsets)+1)
					subsets[0] = "None"
					index := 1
					for subset := range(preset.Subsets) {
						subsets[index] = subset
						index++
					}
					subset := widget.NewSelect(subsets, func(v string) {
						switch v {
						case "None":
							delete(presets_d, preset_id)
						default:
							presets_d[preset_id] = v
						}
					})
					subset.SetSelected("None")
					p.Add(subset)
					p.Add(controls)
				} else {
					p.Add(controls)
				}
					
				presets_c[preset_id] = p_enabled
				if preset.Id != 0 && preset.Id < len(add_order) {
					add_order[preset.Id-1] = p_disabled // TODO: Deal with confliction later
				} else {
					for i := len(add_order)-1; i >= 0; i-- {
						if add_order[i] == nil {
							add_order[i] = p_disabled
							break
						}
					}
				}
			}
			for _, item := range(add_order) {
				disabled.Add(item)
			}
			add_order = nil
			d := dialog.NewCustomConfirm("Neyulw - Presets", "Apply", "Cancel", container.NewGridWithColumns(2, container.NewVScroll(disabled), container.NewVScroll(enabled)), func(v bool) {
				if v {
					for k := range(uconf.Options) {
						remove_user_option(k)
					}
					for _, item := range(presets_b) {
						options := conf.Presets[item].Options
						for option_id, option_value := range(options) {
							if option, ok := option_map[option_id]; ok {
								add_user_option(option_id, option.File, option.Section, option_value)
							}
						}
						if subset, ok := presets_d[item]; ok {
							options := conf.Presets[item].Subsets[subset].Options
							for option_id, option_value := range(options) {
								if option, ok := option_map[option_id]; ok {
									add_user_option(option_id, option.File, option.Section, option_value)
								}
							}
						}
					}
				}
			}, window)
			d.Resize(window.Canvas().Size())
			d.Show()
		})
		save := widget.NewButton("Save", func() {
			files := make(map[string]struct{data *ini.File; file string}, len(conf.Targets))
			for file, path := range(conf.Targets) {
				files[file] = struct{data *ini.File; file string}{data: ini.Empty(ini.LoadOptions{Insensitive: false, AllowShadows: false, AllowNonUniqueSections: false}),
					file: os.Expand(path, func(v string) string {
						if x, ok := vars[v]; ok {
							return x
						}
						return ""
					})}
				dir := filepath.Dir(files[file].file)
				if !pathExists(dir) {
					os.MkdirAll(dir, os.ModePerm)
				}
			}
			for option_id, option := range(uconf.Options) {
				section, _ := files[option.File].data.NewSection(option.Section)
				section.NewKey(option_id, option.Value)
			}
			for _, file := range(files) {
				err := file.data.SaveTo(file.file)
				if err != nil {
					panic(err)
				}
			}
		})
		quit := widget.NewButton("Quit", func() {
			app.Quit()
		})
		window.SetOnClosed(quit.OnTapped)
		c := container.NewCenter(container.NewVBox(logo, presets, save, quit))
		content[0] = c
	}
	{
		var enabled_toggle, search_hints_toggle *NeyulwToggleButton
		enabled_toggle = NewToggleButton("", "Only Enabled", func(s bool) {
			search_only = 1;
			search.OnChanged(search.Text)
		})
		search_hints_toggle = NewToggleButton("", "Search Hints", func(s bool) {
			search.OnChanged(search.Text)
		})
		search = widget.NewEntry()
		search.OnChanged = func(text string) {
			switch search_only {
			case 1:
				search1(text, enabled_toggle.state, search_hints_toggle.state)
			case 2:
				search2(text, enabled_toggle.state, search_hints_toggle.state)
			default:
				search1(text, enabled_toggle.state, search_hints_toggle.state)
				search2(text, enabled_toggle.state, search_hints_toggle.state)
			}
			search_only = 0;
		}
		content[1] = container.NewBorder(nil, container.NewGridWithColumns(2, enabled_toggle, search_hints_toggle), nil, nil, search)
	}
	{
		var items_mutex sync.Mutex
		items := make(map[string]struct{object fyne.CanvasObject; toggle *NeyulwToggleButton; hint *widget.Label; bg *canvas.Rectangle})
		items_container := container.NewVBox()

		var items_sync sync.WaitGroup
		items_sync.Add(len(option_map))
		for option, info := range(option_map) {
			go func() {
				name := widget.NewLabel(option)
				name.Wrapping = fyne.TextWrapBreak
				var option_toggle *NeyulwToggleButton
				option_toggle = NewToggleButton("On", "Off", func(s bool) {
					if s {
						add_user_option(option, info.File, info.Section, "")
					} else {
						remove_user_option(option)
					}
				})
				file_label := widget.NewLabel(info.File)
				file_label.Truncation = fyne.TextTruncateEllipsis
				section_label := widget.NewLabel(info.Section)
				section_label.Alignment = fyne.TextAlignTrailing
				section_label.Truncation = fyne.TextTruncateEllipsis
				bg := canvas.NewRectangle(color.Black)
				
				items_mutex.Lock()
				if _, ok := uconf.Options[option]; ok {
					option_toggle.state = true
					add_user_option_(option, info.File, info.Section, "")
				}
				if info.Hint != "" {
					hint := widget.NewLabel(info.Hint)
					hint.Wrapping = fyne.TextWrapBreak
					items[option] = struct{object fyne.CanvasObject; toggle *NeyulwToggleButton; hint *widget.Label; bg *canvas.Rectangle}{container.NewStack(bg, container.NewBorder(nil, container.NewVBox(hint, container.NewGridWithColumns(2, file_label, section_label)), nil, nil, container.NewGridWithColumns(2, name, option_toggle))), option_toggle, hint, bg}
				} else {
					items[option] = struct{object fyne.CanvasObject; toggle *NeyulwToggleButton; hint *widget.Label; bg *canvas.Rectangle}{container.NewStack(bg, container.NewBorder(nil, container.NewGridWithColumns(2, file_label, section_label), nil, nil, container.NewGridWithColumns(2, name, option_toggle))), option_toggle, nil, bg}
				}
				items_mutex.Unlock()
				items_sync.Done()
			}()
		}
		items_sync.Wait()

		search1 = func(text string, enabled, hints bool) {
			items_container.RemoveAll()
			i := 0
			value := strings.ToLower(text)

			for option, v := range(items) {
				if _, ok := uconf.Options[option]; ok != v.toggle.state {
					v.toggle.internal()
				}
				if enabled {
					if _, ok := uconf.Options[option]; !ok {
						continue
					}
				}
				if hints && v.hint != nil {
					if !strings.Contains(strings.ToLower(v.hint.Text), value) && !strings.Contains(strings.ToLower(option), value) {
						continue
					}
				} else if !strings.Contains(strings.ToLower(option), value) {
					continue
				}
				if i >= 15 {
					break
				}
				if i%2 == 1 {
					v.bg.FillColor = color.RGBA{80,80,80,255}
				} else {
					v.bg.FillColor = color.RGBA{50,50,50,255}
				}
				items_container.Add(v.object)
				i++
			}
			if i >= 15 {
				omitted := widget.NewLabel(".. Omitted ..")
				omitted.Alignment = fyne.TextAlignCenter
				items_container.Add(widget.NewSeparator())
				items_container.Add(omitted)
			}
			items_container.Refresh()
		}
		search_only = 1
		search.OnChanged("")
		content[2] = container.NewBorder(nil, nil, nil, nil, items_container)
	}
	{
		items_container := container.NewVBox()
		search2 = func(text string, _, hints bool) {
			items_container.RemoveAll()
			t := strings.ToLower(text)
			for option, v := range(user_options) {
				if hints {
					if o2, ok := option_map[option]; (!ok || !strings.Contains(strings.ToLower(o2.Hint), t)) && !strings.Contains(strings.ToLower(option), t) {
						continue;
					}
				} else if !strings.Contains(strings.ToLower(option), t) {
					continue
				}
				items_container.Add(v.object)
			}
			items_container.Refresh()
		}
		
		// For future user conf loading
		for _, object := range(user_options) {
			items_container.Add(object.object)
		}

		add_user_option = func(option, file, section, value string) {
			add_user_option_(option, file, section, value)
			// TODO: Handle search text
			items_container.Add(user_options[option].object)
			items_container.Refresh()
		}
		remove_user_option = func(option string) {
			if obj, ok := user_options[option]; ok {
				items_container.Remove(obj.object)
				items_container.Refresh()
			}
			remove_user_option_(option)
		}

		content[3] = container.NewBorder(nil, nil, nil, nil, items_container)
	}

	window.SetContent(container.NewBorder(nil, nil, content[0], nil, container.NewBorder(content[1], nil, nil, nil, container.NewGridWithColumns(2, container.NewVScroll(content[2]), container.NewVScroll(content[3])))))
	return window
}

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	setup()
	ui := app.NewWithID("Neyulw")
	ui.Settings().SetTheme(&NeyulwTheme{})
	w_init := ui.NewWindow("Neyulw - Hey!")
	{
		content := container.New(layout.NewVBoxLayout())
		c_names := make([]string, len(configs))
		{
			i := 0
			for k := range configs {
				c_names[i] = k
				i++
			}
		}
		var c *widget.Select
		check := func() bool {
			return false
		}
		vars := func() map[string]string {
			return make(map[string]string)
		}
		edit := widget.NewButton("Configure", func() {
			if check() {
				w_init.Hide()
				win := configureGame(ui, configs[c.Selected], nil, vars())
				win.Show()
			}
		})
		c = widget.NewSelect(c_names, func(name string) {
			entries := make(map[string]*widget.Entry, len(configs[name].Vars))
			content.RemoveAll()
			content.Add(c)
			content.Add(widget.NewSeparator())
			for v_id, v := range(configs[name].Vars) {
				le := widget.NewEntry()
				le.OnChanged = func(text string) {
					le.Text = text
				}
				entries[v_id] = le
				switch v.Type {
				case "dir":
					b := widget.NewButtonWithIcon("", theme.FolderIcon(), func() {
						folder := folderDialog(fmt.Sprintf("Neyulw - Select %s", v.Name))
						if folder != "" {
							le.SetText(folder)
						}
					})
					sel := container.NewBorder(nil, nil, nil, b, le)
					if v.Hint == "" {
						content.Add(container.NewVBox(sel))
					} else {
						hint := widget.NewLabel(v.Hint)
						content.Add(container.NewVBox(sel,hint))
					}
				default:
					b := widget.NewButtonWithIcon("", theme.FileIcon(), func() {
						file := fileDialog(fmt.Sprintf("Neyulw - Select %s", v.Name), 0)[0]
						if file != "" {
							le.SetText(file)
						}
					})
					sel := container.NewBorder(nil, nil, nil, b, le)
					if v.Hint == "" {
						content.Add(container.NewVBox(sel))
					} else {
						hint := widget.NewLabel(v.Hint)
						content.Add(container.NewVBox(sel,hint))
					}
				}
			}
			check = func() bool {
				for _,v := range(entries) {
					if v.Text == "" || !pathExists(v.Text) {
						return false
					}
				}
				return true
			}
			vars = func() map[string]string {
				vars_ := make(map[string]string)
				for v_id, v := range(entries) {
					vars_[v_id] = filepath.Clean(v.Text)
				}
				return vars_
			}
			content.Add(edit)
			content.Refresh()
		})
		c.SetSelected(c.Options[0])
		w_init.SetContent(content)
	}

	w_init.CenterOnScreen()
	w_init.Show()
	ui.Run()
}
