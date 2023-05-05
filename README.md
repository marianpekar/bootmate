# Bootmate

Bootmate is a lightweight automation tool that accepts via command line arguments a recipe file, path to an executable with any command line arguments for this executable and then it runs this executable and performs actions from the recipe.

Or you can run Bootmate with just a path to a recipe. In this case, no executable will be run and Bootmate starts performing the actions immediately.

You can also run executables from a recipe itself, using a `run` command (see [All Recipe Commands](#all-recipe-commands)).

```
Bootmate.exe [recipe] ([executable]) ([command-line arguments])
```

A recipe file might look like this:
```
var:name Bootmate
var:half_second 500

loop:2
  write:Knock...
  press:enter
end

write:Who's there?
sleep:${$half_second*2}
press:enter
write:It's $name!
sleep:$half_second
press:enter
write:Boot, who?
name:Boot Mate
sleep:$half_second
press:enter
write:$name!

hold:ctrl
press:s
release:ctrl
sleep:1200
write:kkjk.txt
press:enter
```
When you run this recipe on a notepad.exe, like this
```
Bootmate.exe example.recipe "C:\Windows\notepad.exe"
```
the process will start the Notepad, type the following text and save it as `kkjk.txt`
```
Knock...
Knock...
Who's there?
It's Bootmate!
Boot, who?
Boot Mate!
````
In the example you can see a usage of `loop`, `write`, `sleep`, `press`, `hold`, and `release` commands, but also `var`iables and [Expressions](#expressions). There are also commands to drive a mouse, like in this example:
```
set cursor:512 400
var:size 100
move cursor:$size 0
move cursor:0 $size
move cursor:${-$size} 0
move cursor:0 ${-$size}
click:left
```
This first sets the cursor position to x 512 and y 400 pixels, then moves it along a square-shaped path clockwise and simulates a LMB click.

On top of that, Bootmate is configurable by `bootmate.ini` you can but don't have to provide, and both ini and recipe file support single-line comments prefixed with `;`. If you need a semicolon you need to escape it with a backtick.

```
var:name Bootname ;this is a comment
name:Bootname`;this is NOT a comment
```
In this^ example, the value of the `name` variable has been re-assigned from `Bootname` to `Bootname;this is NOT a comment`

## All Recipe Commands
Each command has to be placed on a separate line. When a command accepts values, values are separated from the command with a `:`. Multiple values are separated from themselves by space.
| Syntax | Description | Example |
| - | - | - |
| write:{text} | Simply types a given text. Default delay between characters is 1 millisecond (see Bootmate.ini). | ```write:hello``` |
| press:{key*} | Makes a single key press. | ```press:enter``` |
| hold:{key*} | Holds a key until `release` is called or program ends. | ```hold:shift``` |
| release:{key*} | Releases a key. If the key is not currently held, nothing happens. | ```release:shift``` |
| set cursor:{x} {y} | Move cursor an absolute position (position [0,0] is the top left corner of the screen). | ```set cursor:512 400``` |
| move cursor:{x} {y} | Move cursor to a relative position. | ```move cursor:0 10``` |
| click:{left/middle/right} | Performs a left, middle, or right mouse button click. | ```click:right``` |
| hold mouse:{left/middle/right} | Holds a mouse button until `release mouse` is called or program ends. | ```hold mouse:left``` |
| release mouse:{left/middle/right} | Releases a mouse button. If the button is not currently held, nothing happens. | ```release mouse:left``` |
| scroll:{amount} | Rolls a mouse wheel. A positive number rolls up (forward), a negative rolls down (backward). | ```scroll:-120``` |
| sleep:{ms} | Waits a given time in milliseconds. | ```sleep:500``` |
| loop:{n} ... end | Repeats commands between `loop` and `end` n-times. | ```loop:10``` <br/>...<br/> ```end``` |
| run:{exe} ({args}) | Runs an executable, optionally with cmd line args. **If your path contains spaces or '`:`' wrap the path with `"`** | ```run:"C:\Program Files\My Precious App\MPA.exe"``` |
| var:{name} ({value}) ** | Declares a variable. You can but don't have to initialize it. The default value is an empty string. | ```var:name Bootmate``` |
| {name}:{value} ** | Re-assign a variable. Value can contain any character, including a space (semicolon needs to be escaped with a `` ` `` as everywhere where you don't want to mark the rest of a line as a comment). | ```name:Boot Mate``` |

*) See the [List of Keys](#list-of-keys)

**) Use variables in values of any command by prefixing its name with a `$` (e.g. `$name`)

## List of Keys
Some virtual keys have two options (the second option is in brackets)
```
return (enter)
shift
ctrl (control)
alt,
backspace
delete
tab
escape (esc)
space
left
right
up
down
home
end
pageup
pagedown
insert
f1 — f12
numpad0 — numpad9
multiply (*)
add (+)
subtract (-)
decimal (.)
divide (/)
0 — 9
a — z
```

 ## Built-in Variables
Bootmates has a couple of built-in variables. Here's a list of them:

| Variable | Description |
| --- | --- |
| `$weekday` | The current day of the week from 1 to 7, where 1 is Sunday and 7 is Saturday. |
| `$day` | The current day of the month |
| `$month` | The current month |
| `$year` | The current year |
| `$hour` | The current hour in UTC, if you need to adjust to your timezone, you can do that with an expression like this `${$hour+2}` |
| `$minute` | The current minute |
| `$second` | The current second |

## Expressions

You can also use basic mathematical expressions (supported operators are `()`, `%`, `^`, `*`, `/`, `+`, and `-`) in any value, an expression has to start with `${` and ends with `}`. Here are some examples:

```
write:${2+4*2} is smaller than ${(2+4)*2} and ${(2+4)*-2} is a negative number.
write:Two to the power of four is ${2^4}.
```
Writes `10 is smaller than 12 and -12 is a negative number. Two to the power of four is 16.`

```
var:pi 3.141592
var:tau ${$pi*2}
write:$tau is twice as large as $pi
```
Writes `6.283184 is twice as large as 3.141592`
```
var:n ${2+4*2} 
n:${$n/2}
loop:${$n+1}
  write:${$n*2} divided by 2 is $n
  press:enter
end
```
 Writes 6 lines of `10 divided by 2 is 5`

 ## Branching
 
You can compare numbers and expressions using common comparison operators* and branch your recipe with `if`, `else if`, and `else` statements. **Notice the whole block has to end with `end if`**.

|*) Comparison Operators | |
|---|---|
|`==`| equal |
|`!=`| not equal |
|`<`| less than |
|`>`| greater than |
|`<=`| less than or equal |
|`=>`| greater than or equal |

Here are a few examples:

 ```
if:${1+1 < 3}
    sleep:5000
end if
write:Five seconds have passed
 ``` 
Because the expression `${1+1 < 3}` evaluates to `true`, program waits 5 seconds and then writes `Five seconds have passed`.

 ```
var:n 1
if:${$n != 1}
    sleep:5000
else
    write:One equals one
end if
 ``` 
 Immediately writes `One equals one`

 ```
 if:${$weekday == 1}
	write:Sunday
else if:${$weekday == 2}
	write:Monday
else if:${$weekday == 3}
	write:Tuesday
else if:${$weekday == 4}
	write:Wednesday
else if:${$weekday == 5}
	write:Thursday
else if:${$weekday == 6}
	write:Friday
else
	write:Saturday
end if
 ```
 Writes the name of the current weekday (`$weekday` is a built-in variable)

## Bootmate.ini
Optional `bootmate.ini` currently supports the following global configuration pairs.

| Key | Description | Default Value |
| - | - | - |
| bNoMultipleInstances | Prevents Bootmate from running an executable if such application is running already | 1 (true)
| bFindWindowByTitle | From some applications Bootmate fails to get a window handle.<br/>You can switch it to try to find a window by its title instead.<br/>This can be also useful when you run Bootmate without passing an executable. | 0 (false) |
| sWindowTitle | A window title relevant to `bFindWindowByTitle` | - |
| iDefaultDelay | A delay between each command in milliseconds | 1 |
| iDefaultWriteDelay | A delay between each character in `write` command in milliseconds | 1 |