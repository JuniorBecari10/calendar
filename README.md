# When

When is a simple CLI app for displaying calendars and managing alarms. <br />
It works offline, and has no dependencies.

## How to use

When is very simple to use. If you run `when help`, it will display this message:
```
Commands:
    (no args)             - show current month
    <year>                - print all months of the specified year
    <month> <year>        - print the specified month of the specified year.

    now                   - show the current day of the week, date and time
    help                  - show this help message
    version               - show version information
    watch                 - watch mode; required for the alarms to ring

    import <file> [-y]    - import calendar from the specified file.
    export <file>         - export calendar to the specified file.

    alarm                 - manage alarms
        add <description> - add a new alarm
            | daily   <hour - hh:mm>
            | weekly  <day of the week - 1-7> <hour - hh:mm>
            | monthly <day of the month> <hour - hh:mm> [--clamp]
            | yearly  <month - 1-12> <day of the month> <hour - hh:mm> [--clamp]
            | once    <year> <month - 1-12> <day of the month> <hour - hh:mm>
                [-y]

        edit <id> <description> - edit an existing alarm
            | daily   <hour - hh:mm>
            | weekly  <day of the week - 1-7> <hour - hh:mm>
            | monthly <day of the month> <hour - hh:mm> [--clamp]
            | yearly  <month - 1-12> <day of the month> <hour - hh:mm> [--clamp]
            | once    <year> <month - 1-12> <day of the month> <hour - hh:mm>

       list [
            | daily
            | weekly
            | monthly
            | yearly
            | once
            ]             - list all the alarms acording to the specified filters

       remove <id> [-y]   - remove the specified alarm
       clear  [-y]        - clear all alarms
```

### Notes
- The `monthly` and `yearly` frequencies have an optional extra `--clamp` flag. It is used to clamp the alarm to the last day of the month, if the current month doesn't have enough days to ring the alarm properly;
- The `yearly` frequency has the `--clamp` flag because of February 29, which only occurs on leap years; in other years the alarm will ring on February 28 instead;
- Some operations, like `remove`, `clear` and `add` have an extra confirmation step, because they are destructive (`add` is not; it is explained better below). You can add an extra flag `-y` to bypass this confirmation step;
- The `add` operation has this confirmation step only when you try to add a duplicate alarm; it reminds the user that there is already another alarm with the same frequency and hour than the one you are trying to add. If confirmed, it will add it, and you will have two alarms ringing in the same time.
- All alarms have a unique ID, which is a four-digit number. It is used to refer to an specific alarm when you edit or remove it.

## Watch mode

The Watch mode is the core of When. It is strictly necessary for the alarms to ring. <br />
Basically, it is a persistent background process that monitors time and rings alarms.

This is an example of the Watch mode screen:
```
Calendar - Watch Mode
Tuesday, April 8, 2025 (2025-04-08) 11:28:37
Press Ctrl-C to exit.

Alarms to ring today:
Go to gym | id: 6880 | Weekly - Tuesdays - 15:30
Go to sleep | id: 2949 | Daily - 22:00
Have lunch | id: 2428 | Daily - 11:30
```

When an alarm is ringing, it looks like this:
```
Calendar - Watch Mode
Tuesday, April 8, 2025 (2025-04-08) 11:30:02
Press Ctrl-C to exit.

Ringing now:
Have lunch | id: 2428 | Daily - 11:30

Alarms to ring today:
Go to gym | id: 6880 | Weekly - Tuesdays - 15:30
Go to sleep | id: 2949 | Daily - 22:00
Have lunch | id: 2428 | Daily - 11:30
```

### How it works

Watch mode consistently monitors time and check if the current time matches with some of your alarms.
It also emits a sound signal, if your terminal supports it, via a special escape sequence: the beep (`\a`).

## Examples

For you to understand better how to use the app, here are some examples: <br />
There will be included the prompt and its response.

### Calendar

- **Printing the current month**
```
$ when
```
```
     April 2025
Su Mo Tu We Th Fr Sa
       1  2  3  4  5 
 6  7  8  9 10 11 12 
13 14 15 16 17 18 19 
20 21 22 23 24 25 26 
27 28 29 30 
```

- **Printing all months of a specific year**

```
$ when 2022
```
```
     Year of 2022

     January 2022
Su Mo Tu We Th Fr Sa
                   1 
 2  3  4  5  6  7  8 
 9 10 11 12 13 14 15 
16 17 18 19 20 21 22 
23 24 25 26 27 28 29 
30 31 

     February 2022
Su Mo Tu We Th Fr Sa
       1  2  3  4  5 
 6  7  8  9 10 11 12 
13 14 15 16 17 18 19 
20 21 22 23 24 25 26 
27 28 

(...)
```


- **Printing a specific month of a specific year**
```
$ when 7 2023
```
```
     July 2023
Su Mo Tu We Th Fr Sa
                   1 
 2  3  4  5  6  7  8 
 9 10 11 12 13 14 15 
16 17 18 19 20 21 22 
23 24 25 26 27 28 29 
30 31 
```
