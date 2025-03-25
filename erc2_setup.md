if you're getting an error about file not found: arduinoavr, here is how I fixed it

1. install the platformio cli (`brew install platformio` on mac and I think you get it [here](https://platformio.org/install/cli) on windows)
    - this does not come with the vs code extension, you have to install it separately
2. run this command: `pio pkg install --global --tool "platformio/framework-arduino-avr"`
3. open platformio.ini in the erc2 template folder
4. replace the line under `platform_packages = ` (line 22 for me) with:
`   framework-arduino-avr@^5.2.0`
5. save the file and it should automatically get the libs

this worked for me because platformio can't get the lib from the internet for some reason, so this solution downloads it locally for pio to use
