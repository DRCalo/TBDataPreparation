#! /usr/bin/env python

import AdcMap24 
import json

class MyClassEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, AdcMap24.ADC):
            return obj.to_dict()
        return super().default(obj)

with open("AdcMap24.json", "w") as json_file:
    json.dump(AdcMap24.adcMapDictionary, json_file, cls=MyClassEncoder, indent=4)
    


