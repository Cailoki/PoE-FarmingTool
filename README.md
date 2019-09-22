
<center> <h2>PoE-FarmingTool</h2> </center>


[Forum](https://www.pathofexile.com/forum/view-thread/2472341) - [Discord](https://discord.gg/D2m7Kpw) - [Patreon](https://www.patreon.com/cailoki) - [Video Guide](https://www.youtube.com/watch?v=GPGDLe2C2QE) - [Screenshot](https://i.imgur.com/xvD6qOT.png)

**NOTE:**\
This is the first project I made while learning C++ programming language. For any tips, suggestions or issues you might have please let me know here or on Discord, I am willing to learn, improve myself and this little tool! ;)

**Whats this all about?**
This is NOT a hax/bot/afk farming tool! Farming maps and looking for neat drops are fun, but there is no tool to keep track of spent/profit currency, so I decided to make one myself. No more calculating on the paper/spreadsheets, keeping track of all loot on a bunch of stash tabs and spending time crunching numbers.

For example, you opened a map, how much currency did you lose? How much did the map cost? Did you use chisels on it, made it rare and/or corrupted? Maybe you put fragment for extra quantity or scarab for extra mods. How about Zana mods? This tool will keep track of all those numbers and display them as spent when you enter a map.

When you finish a map, moving items to stash will add values to profit. In the end, you will know if it paid off or not!

**How to use this tool?**\
Quick guide: Check league, update currency, use CTRL + LMB(Left Mouse Button) to transfer a map to the device, enter it and see if "spent" is showing value, all loot from the map transfer to the stash with CTRL+LMB and look for profit value, repeat! =)

On startup, there will be a warning for missing currency rates and/or missing Client.txt (depends on where the game is installed)!
1. Make sure the league is set to the one you want to use the tool in, Settings -> League. (won't work for standard leagues)
2. Navigate to currency menu and click update (this will pull prices locally and make parsing faster, be sure to update rates every few hours if you are playing a lot :)
3. Go to the settings -> map:\
  -for farming specific maps put in map name, for example (Toxic Sewer), all loot outside that map will be ignored\
  -map price: if set to 0, use online price, else use manually set price\
  -add Zana mod price if used\
  -general: any opened map will be displayed as spent and all loot added to profit\
  -ignore trash: values of 1c or below will not be added to profit, like trash divination cards, low tier maps...
4. IIQ is unused at the moment but can be edited and displayed on the main window just as info
5. Sextants: in format 0-0-0 (Apprentice-Journeyman-Master), the average price will be calculated and on each sextant use that average will be added to the spent value
6. Overlay settings are screen coordinates, top-left corner is 0, top-right is 1, bottom-left is 1, if using two displays, values go up to 2 (play with settings and find what best suits you, for me (0.85 and 0.4 - below minimap) or (0.3 and 0.91 - above exp bar) looks fine)
7. Under the "File" menu there is a reset data option which resets runs, spent and profit to 0
8. +Profit button adds set value to profit
9. +Spent button adds set value to spent

**Note:**\
10. Always transfer items to stash and/or map device using Ctrl+LMB otherwise tool won't get their values\
11. Holding down Ctrl and clicking LMB(Left Mouse Button) will trigger parsing of an item below the cursor, this hotkey also transfers items between the stash and the inventory so you don't have to drag and drop loot.\
12. Holding down CTRL and clicking RMB(Right Mouse Button) will disable the tool and play little notification beep, all parsing is disabled from that point (useful when moving between hideouts, farming in friends maps, doing lab or temple)\
13. Always put Map first in the map device and then fragments/scarabs, anything else will break the chain and won't calculate exact numbers
 
**Additional Notes:**\
-Make sure to play Path of Exile in English, if you're using a different language it probably won't work.\
-At the beginning of the league, prices are very volatile and incorrect, especially prices of currency (scrolls are not worth 1 Chaos Orb!)\
-When moving a map to the map device, it's price will be added to profit, if that map is opened (entered), price will be added to the spent value and subtracted from profit twice(to cancel first add as profit, and again to show loss of currency)\
-At the moment this tool and PoE-TradeMacro share Ctrl+C hotkey (or rather clipboard data), moving items to the stash will show item info window