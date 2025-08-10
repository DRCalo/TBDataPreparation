import csv
import json
import argparse

''' This script is meant to take a simple csv format in input (with the following structure:
    Tower,S_pk,C_pk,S_ped,C_ped
    ) and convert to a json file to be used by PhysicsEvent,h for the calibration. 
    The use of a csv file is meant to simplify teh work to whoever will do the analysis to find teh    pedestals and peaks

    Prepared by Iacopo Vivarelli - 31/08/2024
'''

class DR_tower:
  ''' Class describing a tower channel (peak, ped)
    - peak     # peak position of teh energy when shooting a beam of electrons in the tower
    - ped   # pedestal from dedicated pedestal runs
  '''
  def __init__(self, peak, ped):
    self.peak     = peak     # peak value 
    self.ped   = ped   # pedestal
    
  def __str__(self):
    return "%4d %8s %4d %4d " % (self.peak, self.ped)
    
  def to_dict(self):
    return {
      'peak':self.peak,
      'ped':self.ped
    }

class MyClassEncoder(json.JSONEncoder):
  def default(self, obj):
    if isinstance(obj, DR_tower):
      return obj.to_dict()
    return super().default(obj)

  
def main():
  parser = argparse.ArgumentParser(description='DoPhysicsConverter - a script to produce the final physics ntuples',formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('-o','--output_file', action='store', dest='name_file_json',
                      default='RunXXX_2024_prelim.json',
                      help='Name of the output json file.')
  parser.add_argument('-i','--input_file', action='store', dest='name_file_csv',
                      default='Equalization-Sheet1.csv',
                      help='Name of the input csv file.')

  par = parser.parse_args() 
  name_file_csv = par.name_file_csv
  name_file_json = par.name_file_json

  # This will store the output dictionary data
  dati = {}

  # Read csv and store in dati
  with open(name_file_csv, mode='r', newline='', encoding='utf-8') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
      tower_name = row['Tower']
      if (tower_name[0] == 'T'):
        s_tower_name = 'TS' + tower_name.split('T')[1]
        c_tower_name = 'TC' + tower_name.split('T')[1]
        dati[s_tower_name] = DR_tower(row["S_pk"],row["S_ped"])
        dati[c_tower_name] = DR_tower(row["C_pk"],row["C_ped"]) 


  # Write on a json file
  with open(name_file_json, mode='w', encoding='utf-8') as jsonfile:
    json.dump(dati, jsonfile, cls = MyClassEncoder,  indent=4, ensure_ascii=False)

  print(f"Conversione done! The JSON file is saved as '{name_file_json}'")


if __name__ == "__main__":
  main()
