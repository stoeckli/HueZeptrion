# HueZeptrion
Control Philips Hue lights with Feller Zeptrion keys and ESP8266

Demo Video on https://youtu.be/jSjQW7vah4E

# Mechanical Switch
This branch includes a sketch for an ESP8266-01 to be hooked up to a mechnical on-off switch. The switch is connected between GPIO0 and GPIO2. On each status change, a senso flag on the bridge is set. To get this working you need:

- Connect to the hue bridge API
- get a new user key (see API docu)
- generate a generic status sensor by posting to "/sensors":
{
	"state": {
		"flag": false
	},
	"config": {
		"on": true,
		"reachable": true
	},
	"name": "ToggleSwitch",
	"type": "CLIPGenericFlag",
	"modelid": "Feller-On-OFF-ON",
	"manufacturername": "Feller",
	"swversion": "1.0",
	"uniqueid": "201801112114"
}

- note the returned sensor ID and modify it in the sketch. Also, poll the scenes and get the ID of the one you want to set in the on-event
- now we need two rules for the toggle events. generate them with the ID of the sensor and the scene:
{
    "name": "Group Off via ESP8266",
    "status": "enabled",
    "conditions": [
        {
            "address": "/sensors/2/state/flag",
            "operator": "eq",
            "value": "true"
        },
        {
            "address": "/sensors/2/state/lastupdated",
            "operator": "dx"
        },
        {
            "address":"/groups/1/state/any_on",
	"operator":"eq",
	"value":"true"
        }
    ],
    "actions": [
        {
            "address": "/groups/1/action",
            "method": "PUT",
            "body": {
                "on": false
            }
        },
        {
            "address": "/sensors/2/state",
            "method": "PUT",
            "body": {
                "flag": false
            }
        }
    ]
}

{
    "name": "Group On via ESP8266",
    "status": "enabled",
    "conditions": [
        {
            "address": "/sensors/2/state/flag",
            "operator": "eq",
            "value": "true"
        },
        {
            "address": "/sensors/2/state/lastupdated",
            "operator": "dx"
        },
        {
            "address":"/groups/1/state/any_on",
	"operator":"eq",
	"value":"false"
        }
    ],
    "actions": [
        {
            "address": "/groups/1/action",
            "method": "PUT",
            "body": {
	"scene": "G2DHKCn3HyRT910"
	}
        },
        {
            "address": "/sensors/2/state",
            "method": "PUT",
            "body": {
                "flag": false
            }
        }
    ]
}

- that was it. to test the rules on the bridge, put this to "/sensor/2"
{
	"state": {
		"flag": true
}}


For OTA to work, Firewall needs to be disabled and Bonjour service. Toggle switch while uploading to enable OTA.
