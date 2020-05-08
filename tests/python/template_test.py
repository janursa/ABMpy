import unittest
from CA import CA
from random import random,randrange

settings = {
  "configs":{
    "exp_duration" : 72,
    "domain":{
        "x_l": 1,
        "y_l": 1,
        "patch_size": 0.1,
        "measurements":[
          {
            "on":"patch",
            "mode": "mean",
            "tags":["patch_key1"]
          },
          {
            "on":"agent",
            "mode": "mean",
            "tags":["agent_key1"]
          }
        ]
    },
    "patch":{
        "attrs":{ "patch_key1":10}
      },
    "agents":
      {
        "agent_name1":{
          "initial_n": 20,
          "attrs":{"agent_key1":2.2}
        },
        "agent_name2":{
          "initial_n": 0,
          "attrs":{}
        }

      }

  },
  "logs":{
    "flag": True,
    "agent": [
      {
        "flag":True,
        "interval":5,
        "type": "scatter",
        "tags":["agent_key1"],
        "dir":"outputs/agents_scatter_data.csv"
      },
      {
        "flag":True,
        "interval":1,
        "type":"agents_count_traj",
        "dir":"outputs/agents_count_data.csv"
      },
      {
        "flag":True,
        "interval":1,
        "tags":["agent_key1"],
        "type":"traj",
        "dir":"outputs/agents_traj_data.csv"
      }
    ],
    "patch":[
      {
        "flag":True,
        "interval":5,
        "type":"densitymap",
        "tags":["patch_key1"],
        "dir":"outputs/patches_densitymap_data.csv"
      },
      {
        "flag":True,
        "interval":1,
        "type":"traj",
        "tags":["patch_key1"],
        "dir":"outputs/patches_lactate_data.csv"
      }

    ]
  }
}

class agent_name2:
    def __init__(self):
        return
    def forward(self):
            
        if random() < 0.01:
            disappear_flag = True
        else:
        	disappear_flag = False
        self.outputs = {"self":{"disappear":disappear_flag}}
        return True

    inputs = {"patch":{},"self":{}}
    outputs = {"self":{"disappear":False}}

class agent_name1:
    def __init__(self):
        return
    def forward(self):
        if (random() < 0.1):
            walk_flag = True
        else:
            walk_flag = True

        if (random() < 0.05):
            hatch_flag = True
        else:
            hatch_flag = False

        if (random() < 0.05):
            switch_tag = "agent_name2"
        else:
            switch_tag = False

        if (random() < 0.2):
            agent_key1_value = self.inputs["self"]["agent_key1"] + 0.2
        else:
            agent_key1_value = self.inputs["self"]["agent_key1"] - 0.2
        if (random() < 0.2):
            patch_key1_value = self.inputs["patch"]["patch_key1"] - 0.2
        else:
            patch_key1_value = self.inputs["patch"]["patch_key1"] + 0.2

        self.outputs = {"self":{"walk":walk_flag, "hatch":hatch_flag,
                                "switch": switch_tag, "agent_key1":agent_key1_value},
                        "patch":{"patch_key1": patch_key1_value}}
        return True

    inputs = {"patch":{"patch_key1": 0},"self":{"agent_key1":0}}
    outputs = {"self":{"walk":False, "hatch":False, "switch": False, "agent_key1": 0},
                "patch":{"patch_key1": 0}}

class Patch:
    def __init__(self):
        return
    def forward(self):
    	patch_key1_value = self.inputs["agent"]["agent_key1"] * randrange(0,2)
    	self.outputs = {"self":{"patch_key1":patch_key1_value}}
    	return True
    inputs = {"agent":{"agent_key1": 0}}
    outputs = {"self":{"patch_key1":0}}

class MainTest(unittest.TestCase):
    def test(self):
    	agent_objects = {"agent_name1":agent_name1(), "agent_name2":agent_name2()}
    	patch_object = Patch()
    	CA_obj = CA(agent_objects,patch_object, settings)
    	self.assertEqual(CA_obj.run(), True)



if __name__ == '__main__':
    unittest.main()