# 🪟 Katlans UI Module — Complete Window & Widget Reference

> Build native desktop apps. All tkinter functionality, Katlans style.
> 

---

## Window Management

```
win = uiwin <"App", 800, 600> ::
uiwin.title <win, "My App"> ::
uiwin.size <win, 800, 600> ::
uiwin.minsize <win, 400, 300> ::
uiwin.maxsize <win, 1920, 1080> ::
uiwin.resize <win, true, true> ::      ~ resizable x, y
uiwin.icon <win, "icon.png"> ::
uiwin.bg <win, "#ffffff"> ::
uiwin.center <win> ::
uiwin.run <win> ::
uiwin.close <win> ::
uiwin.destroy <win> ::
uiwin.min <win> ::
uiwin.max <win> ::
uiwin.restore <win> ::
uiwin.opacity <win, 0.9> ::
uiwin.full <win, true> ::
uiwin.ontop <win, true> ::
uiwin.geometry <win, "800x600+100+100"> ::
uiwin.state <win> ::                   ~ normal/iconic/zoomed
uiwin.protocol <win, "WM_DELETE_WINDOW", fxn onClose> ::
uiwin.after <win, 1000, fxn tick> ::
uiwin.after.cancel <win, id> ::
uiwin.update <win> ::
uiwin.update.idle <win> ::
uiwin.loop <win> ::
uiwin.quit <win> ::

~ Secondary window
win2 = uitop <win> ::
uitop.title <win2, "Popup"> ::
uitop.modal <win2, win> ::
uitop.transient <win2, win> ::

~ Window Info
int w = uiinfo.width <win> ::
int h = uiinfo.height <win> ::
int x = uiinfo.x <win> ::
int y = uiinfo.y <win> ::
int sw = uiinfo.screenwidth <win> ::
int sh = uiinfo.screenheight <win> ::
int dpi = uiinfo.dpi <win> ::
x = uiinfo.children <win> ::
x = uiinfo.parent <win> ::
true/false = uiinfo.exists <win> ::
true/false = uiinfo.visible <win> ::
```

---

## Variables (like StringVar/IntVar)

```
sv = uivar.str <"default"> ::
iv = uivar.int <0> ::
fv = uivar.flt <0.0> ::
bv = uivar.bool <false> ::

uivar.set <sv, "hello"> ::
x = uivar.get <sv> ::
uivar.trace <sv, "write", fxn onChange> ::
uivar.trace <sv, "read", fxn onRead> ::
uivar.untrace <sv, "write"> ::
```

---

## Label

```
lb = uilabel <win, "Hello"> ::
uilabel.text <lb, "New text"> ::
uilabel.font <lb, "Arial", 14, "bold"> ::
uilabel.color <lb, "#333"> ::
uilabel.bg <lb, "#fff"> ::
uilabel.img <lb, "img.png"> ::
uilabel.imgtext <lb, "img.png", "right"> ::
uilabel.wrap <lb, 200> ::
uilabel.justify <lb, "left"> ::          ~ left/right/center
uilabel.anchor <lb, "nw"> ::             ~ n/s/e/w/nw/ne/sw/se/center
uilabel.relief <lb, "flat"> ::           ~ flat/raised/sunken/groove/ridge
uilabel.border <lb, 2> ::
uilabel.pad <lb, 10, 5> ::
uilabel.cursor <lb, "hand2"> ::
uilabel.width <lb, 20> ::
uilabel.height <lb, 2> ::
uilabel.bind <lb, "var", sv> ::
x = uilabel.get <lb> ::
uilabel.config <lb, "bg", "#red"> ::
x = uilabel.cget <lb, "bg"> ::
```

---

## Button

```
btn = uibtn <win, "Click"> ::
uibtn.text <btn, "New Label"> ::
uibtn.font <btn, "Arial", 12, "bold"> ::
uibtn.color <btn, "#fff"> ::
uibtn.bg <btn, "#4CAF50"> ::
uibtn.activebg <btn, "#45a049"> ::
uibtn.activecolor <btn, "#fff"> ::
uibtn.size <btn, 100, 40> ::
uibtn.width <btn, 20> ::
uibtn.height <btn, 2> ::
uibtn.state <btn, "disabled"> ::         ~ normal/disabled/active
uibtn.img <btn, "icon.png"> ::
uibtn.imgside <btn, "icon.png", "left"> ::
uibtn.relief <btn, "raised"> ::
uibtn.border <btn, 2> ::
uibtn.cursor <btn, "hand2"> ::
uibtn.pad <btn, 10, 5> ::
uibtn.anchor <btn, "center"> ::
uibtn.repeat <btn, 500, 100> ::
uibtn.on <btn, "click", fxn handle> ::
uibtn.invoke <btn> ::                    ~ simulate click
uibtn.flash <btn> ::
uibtn.config <btn, "bg", "#ff0000"> ::
x = uibtn.cget <btn, "text"> ::
```

---

## Entry (Single Line Input)

```
en = uientry <win> ::
uientry.placeholder <en, "Type here..."> ::
uientry.font <en, "Arial", 12> ::
uientry.width <en, 200> ::
uientry.state <en, "readonly"> ::        ~ normal/disabled/readonly
uientry.show <en, "*"> ::               ~ password field
uientry.justify <en, "left"> ::
uientry.relief <en, "sunken"> ::
uientry.bg <en, "#fff"> ::
uientry.color <en, "#000"> ::
uientry.selectbg <en, "#4CAF50"> ::
uientry.bind <en, "var", sv> ::
x = uientry.get <en> ::
uientry.set <en, "text"> ::
uientry.clear <en> ::
uientry.insert <en, 0, "text"> ::
uientry.delete <en, 0, "end"> ::
uientry.select <en, 0, 5> ::
uientry.selectall <en> ::
uientry.selectclear <en> ::
x = uientry.selection <en> ::
int pos = uientry.cursor.pos <en> ::
uientry.cursor.set <en, 5> ::
uientry.validate <en, "key", fxn validateFn> ::
uientry.on <en, "change", fxn handle> ::
uientry.on <en, "focus", fxn handle> ::
uientry.on <en, "enter", fxn handle> ::
```

---

## Text (Multiline)

```
tx = uitext <win, 400, 200> ::
uitext.font <tx, "Courier", 12> ::
uitext.bg <tx, "#fff"> ::
uitext.state <tx, "disabled"> ::
uitext.wrap <tx, "word"> ::              ~ none/char/word
uitext.spacing <tx, 2, 0, 2> ::
uitext.undo <tx, true> ::
uitext.maxundo <tx, 20> ::
uitext.insert <tx, "end", "Hello\n"> ::
x = uitext.get <tx, "1.0", "end"> ::
uitext.delete <tx, "1.0", "end"> ::
uitext.clear <tx> ::
uitext.scrollbar <tx, "both"> ::
uitext.scroll.to <tx, 0.5> ::
uitext.see <tx, "end"> ::
uitext.search <tx, "word", "1.0"> ::
uitext.replace <tx, "old", "new"> ::

~ Tags (formatting)
uitext.tag.add <tx, "bold", "1.0", "1.10"> ::
uitext.tag.config <tx, "bold", "font", "Arial 14 bold"> ::
uitext.tag.config <tx, "highlight", "bg", "#ffff00"> ::
uitext.tag.remove <tx, "bold", "1.0", "end"> ::
uitext.tag.bind <tx, "bold", "click", fxn handle> ::

~ Embed inside text
uitext.embed.img <tx, "end", "icon.png"> ::
uitext.embed.widget <tx, "end", btn> ::

uitext.edit.undo <tx> ::
uitext.edit.redo <tx> ::
```

---

## Frame & LabelFrame

```
fr = uiframe <win> ::
uiframe.bg <fr, "#f0f0f0"> ::
uiframe.size <fr, 400, 300> ::
uiframe.border <fr, 2> ::
uiframe.relief <fr, "groove"> ::
uiframe.pad <fr, 10, 10> ::

lfr = uilabelframe <win, "Settings"> ::
uilabelframe.font <lfr, "Arial", 12, "bold"> ::
uilabelframe.color <lfr, "#333"> ::
```

---

## Checkbox

```
cb = uicheck <win, "Enable"> ::
uicheck.var <cb, bv> ::
uicheck.state <cb, true> ::
uicheck.onval <cb, "yes"> ::
uicheck.offval <cb, "no"> ::
uicheck.tristate <cb, true> ::
true/false = uicheck.get <cb> ::
uicheck.set <cb, true> ::
uicheck.toggle <cb> ::
uicheck.on <cb, "change", fxn handle> ::
```

---

## Radio Button

```
rb1 = uiradio <win, "Option A", "group1", "A"> ::
rb2 = uiradio <win, "Option B", "group1", "B"> ::
uiradio.var <rb1, sv> ::
uiradio.state <rb1, "disabled"> ::
uiradio.on <rb1, "change", fxn handle> ::
x = uiradio.get <"group1"> ::
uiradio.set <"group1", "B"> ::
```

---

## Combobox / Dropdown

```
dd = uidrop <win, "A", "B", "C"> ::
uidrop.state <dd, "readonly"> ::
uidrop.values <dd, "X", "Y", "Z"> ::
uidrop.select <dd, 0> ::
uidrop.add <dd, "D"> ::
uidrop.remove <dd, "A"> ::
x = uidrop.get <dd> ::
uidrop.on <dd, "change", fxn handle> ::
```

---

## Listbox

```
lbx = uilist <win> ::
uilist.selectmode <lbx, "single"> ::     ~ single/multiple/extended
uilist.scrollbar <lbx, "both"> ::
uilist.add <lbx, "Item 1"> ::
uilist.addall <lbx, myList> ::
uilist.insert <lbx, 0, "First"> ::
uilist.remove <lbx, 0> ::
uilist.clear <lbx> ::
x = uilist.get <lbx> ::
int n = uilist.count <lbx> ::
uilist.select <lbx, 0> ::
uilist.sort <lbx> ::
uilist.on <lbx, "select", fxn handle> ::
uilist.on <lbx, "doubleclick", fxn handle> ::
```

---

## Scrollbar

```
sb = uiscroll <win, "vertical"> ::
uiscroll.attach <sb, widget> ::
uiscroll.set <sb, 0.2, 0.8> ::
```

---

## Slider / Scale

```
sl = uislider <win, 0, 100> ::
uislider.orient <sl, "horizontal"> ::
uislider.val <sl, 50> ::
uislider.step <sl, 1> ::
uislider.tickinterval <sl, 10> ::
uislider.length <sl, 200> ::
uislider.troughcolor <sl, "#ddd"> ::
uislider.showvalue <sl, true> ::
uislider.var <sl, iv> ::
x = uislider.get <sl> ::
uislider.on <sl, "change", fxn handle> ::
```

---

## Spinbox

```
sp = uispin <win, 0, 100> ::
uispin.val <sp, 0> ::
uispin.step <sp, 1> ::
uispin.wrap <sp, true> ::
uispin.state <sp, "readonly"> ::
uispin.values <sp, "A", "B", "C"> ::
x = uispin.get <sp> ::
uispin.on <sp, "change", fxn handle> ::
uispin.up <sp> ::
uispin.down <sp> ::
```

---

## Progress Bar

```
pb = uiprog <win, 0, 100> ::
uiprog.mode <pb, "determinate"> ::       ~ determinate/indeterminate
uiprog.set <pb, 75> ::
uiprog.step <pb, 10> ::
uiprog.start <pb, 10> ::
uiprog.stop <pb> ::
uiprog.color <pb, "#4CAF50"> ::
```

---

## Canvas (Drawing)

```
cv = uicanvas <win, 500, 400> ::
uicanvas.bg <cv, "#ffffff"> ::
uicanvas.scrollregion <cv, 0, 0, 1000, 1000> ::

id = uicanvas.line <cv, 0,0, 100,100> color<"#f00"> width<2> ::
id = uicanvas.rect <cv, 10,10, 200,100> fill<"#00f"> outline<"#000"> ::
id = uicanvas.oval <cv, 10,10, 100,100> fill<"#0f0"> ::
id = uicanvas.circle <cv, 100,100, 50> fill<"#f00"> ::
id = uicanvas.arc <cv, 10,10,100,100> start<0> extent<180> ::
id = uicanvas.polygon <cv, 0,0, 50,100, 100,0> fill<"#ff0"> ::
id = uicanvas.text <cv, 50, 50, "Hello"> font<"Arial 14"> fill<"#000"> ::
id = uicanvas.img <cv, 0, 0, "img.png"> ::
id = uicanvas.widget <cv, 100, 100, btn> ::

uicanvas.move <cv, id, 10, 10> ::
uicanvas.moveto <cv, id, 100, 100> ::
uicanvas.delete <cv, id> ::
uicanvas.delete <cv, "all"> ::
uicanvas.raise <cv, id> ::
uicanvas.lower <cv, id> ::
uicanvas.config <cv, id, "fill", "#ff0000"> ::
lt ids = uicanvas.find <cv, "overlapping", 0, 0, 100, 100> ::
x, y, w, h = uicanvas.bbox <cv, id> ::
uicanvas.on <cv, id, "click", fxn handle> ::
uicanvas.on <cv, "motion", fxn handle> ::
```

---

## Image

```
im = uiimg.open <"photo.png"> ::
uiimg.resize <im, 200, 200> ::
uiimg.crop <im, 0, 0, 100, 100> ::
uiimg.rotate <im, 90> ::
uiimg.flip <im, "h"> ::
uiimg.filter <im, "blur"> ::
uiimg.filter <im, "sharpen"> ::
uiimg.filter <im, "grayscale"> ::
uiimg.paste <im, im2, 10, 10> ::
uiimg.save <im, "out.png"> ::
int w, int h = uiimg.size <im> ::
```

---

## Menu Bar

```
mb = uimenu <win> ::
m = uimenu.add <mb, "File"> ::
uimenu.item <m, "New", fxn newFile> accel<"Ctrl+N"> ::
uimenu.item <m, "Open...", fxn openFile> ::
uimenu.sep <m> ::
uimenu.item <m, "Exit", fxn quit> ::
uimenu.check <m, "Dark Mode", bv, fxn toggle> ::
uimenu.radio <m, "Small", sv, "small", fxn setSize> ::
sub = uimenu.sub <m, "Export"> ::
uimenu.item <sub, "As PDF", fxn exportPDF> ::
uimenu.disable <m, "Save"> ::
uimenu.enable <m, "Save"> ::
uimenu.post <m, x, y> ::
```

---

## Notebook (Tabs)

```
nb = uinote <win> ::
uinote.add <nb, fr1, "Tab 1"> ::
uinote.insert <nb, 0, fr3, "First Tab"> ::
uinote.remove <nb, 0> ::
uinote.select <nb, 0> ::
int n = uinote.count <nb> ::
uinote.tab <nb, 0, "text", "New Name"> ::
uinote.tab <nb, 0, "state", "disabled"> ::
uinote.hide <nb, 0> ::
uinote.move <nb, 0, 2> ::
uinote.on <nb, "change", fxn handle> ::
```

---

## Treeview (Table / Tree)

```
tv = uitree <win> ::
uitree.cols <tv, "Name", "Age", "City"> ::
uitree.heading <tv, "Name", "Full Name"> cmd<fxn sortByName> ::
uitree.colwidth <tv, "Name", 150> ::
uitree.colanchor <tv, "Age", "center"> ::
uitree.show <tv, "headings"> ::
uitree.height <tv, 10> ::
uitree.selectmode <tv, "browse"> ::
uitree.scrollbar <tv, "both"> ::

id = uitree.add <tv, "", "end", "John", 25, "Delhi"> ::
id2 = uitree.add <tv, id, "end", "Child"> ::
uitree.set <tv, id, "Name", "Jane"> ::
x = uitree.get <tv, id, "Name"> ::
uitree.delete <tv, id> ::
uitree.clear <tv> ::

lt sel = uitree.selection <tv> ::
uitree.select <tv, id> ::
uitree.open <tv, id, true> ::
lt children = uitree.children <tv, id> ::

uitree.tag.config <tv, "highlight", bg<"#ffff00"> color<"#000">> ::
uitree.tag.bind <tv, "highlight", "click", fxn handle> ::

uitree.on <tv, "select", fxn handle> ::
uitree.on <tv, "doubleclick", fxn handle> ::
uitree.sort <tv, "Name", "asc"> ::
```

---

## PanedWindow

```
pw = uipane <win, "horizontal"> ::
uipane.add <pw, fr1> ::
uipane.add <pw, fr2> minsize<100> ::
uipane.sashpos <pw, 0, 300> ::
```

---

## Dialogs

```
uidialog.info <"Title", "Message"> ::
uidialog.warn <"Title", "Warning!"> ::
uidialog.error <"Title", "Error!"> ::
true/false = uidialog.ask <"Title", "Are you sure?"> ::
x = uidialog.askyesnocancel <"Title", "Save?"> ::

x = uidialog.openfile <"Select File", ".kl", ".txt"> ::
lt files = uidialog.openfiles <"Select Files"> ::
x = uidialog.savefile <"Save As", ".kl"> ::
x = uidialog.folder <"Select Folder"> ::
x = uidialog.color <"#ffffff"> ::
x = uidialog.font :: 
x = uidialog.input <"Enter name:"> ::
x = uidialog.inputpass <"Enter password:"> ::
```

---

## Tooltip & Separator

```
uitooltip <btn, "This is a button"> ::
uitooltip.delay <btn, 500> ::
uitooltip.style <btn, "bg", "#333"> ::

sep = uisep <win, "horizontal"> ::
sep = uisep <fr, "vertical"> ::
```

---

## Fonts & Themes

```
f = uifont <"Arial", 14, "bold"> ::
uitext.measure <f, "Hello World"> ::
lt families = uifont.families <win> ::

uitheme <"dark"> ::                       ~ dark/light/system/clam/alt
uistyle.config <"TButton", bg<"#4CAF50"> color<"#fff">> ::
uistyle.map <"TButton", bg<"active", "#45a049">> ::
```

---

## Layout

```
~ Pack
uipack <widget, side<"top"> fill<"x"> expand<true> padx<10> pady<5>> ::
uipack.forget <widget> ::

~ Grid
uigrid <widget, row<0> col<0> rowspan<2> colspan<2> sticky<"nsew"> padx<5> pady<5>> ::
uigrid.columnconfigure <win, 0, weight<1>> ::
uigrid.rowconfigure <win, 0, weight<1>> ::

~ Place (absolute)
uiplace <widget, x<100> y<200> width<80> height<30>> ::
uiplace <widget, relx<0.5> rely<0.5> anchor<"center">> ::
```

---

## Events & Bindings

```
~ Keys
uion <win, "<Return>", fxn handle> ::
uion <win, "<Escape>", fxn handle> ::
uion <win, "<Control-s>", fxn handle> ::
uion <win, "<F1>", fxn handle> ::
uion <win, "<Up>", fxn handle> ::

~ Mouse
uion <win, "click", fxn handle> ::
uion <win, "doubleclick", fxn handle> ::
uion <win, "rightclick", fxn handle> ::
uion <win, "motion", fxn handle> ::
uion <win, "scroll", fxn handle> ::
uion <win, "drag", fxn handle> ::

~ Window
uion <win, "resize", fxn handle> ::
uion <win, "focus", fxn handle> ::
uion <win, "close", fxn handle> ::
uion <win, "destroy", fxn handle> ::

~ Event object
~ event.x event.y event.keysym event.char event.widget event.delta

uioff <win, "click"> ::
uifocus <widget> ::
x = uifocus.get <win> ::

~ Drag and Drop
uidnd.source <widget, fxn getData> ::
uidnd.target <widget, fxn onDrop> ::

~ Clipboard
uiclip.set <"copied text"> ::
x = uiclip.get <win> ::
uiclip.clear <win> ::
```