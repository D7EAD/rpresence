<h1 align="center">rpresence</h1>
<p align="center">Command-line tool for setting any custom Discord Rich Presence status.</p>
<hr>
rpresence is a simple and easy-to-use command-line utility to set a custom Rich Presence on your Discord profile for all your friends to see. Before using represence, you must have an application registered at Discord's <a href="https://discord.com/developers/applications">Developer Portal</a>. After registering an application, take note of your Application ID.
<br><br>
In order to use rpresence, several pieces of information are required--you can find more information below in the help menu:
<img src="/images/help.jpg">
<br>
Instead of using rpresence via its command-line options, you can simply pass a JSON file in the following format to rpresence via the `-f` parameter:

```json
{
    "applicationID": "application_id_here",
    "state": "sample_state_text",
    "details": "sample_details_text",
    "largeImageKey": "sample_large_image_keyname",
    "largeImageText": "sample_large_image_text",
    "smallImageKey": "sample_small_image_keyname",
    "smallImageText": "sample_small_image_text"
}
```
<br>
<b>NOTE:</b>
When passing long sentences to parameters `-imgSText` and `-imgLText`, be sure to enclose provided text in quotes ("") to ensure the sentence is not split via a space delimiter.
