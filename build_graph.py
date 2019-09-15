#!/usr/bin/env python
import matplotlib
matplotlib.use('Agg')

import argparse
import sys
import matplotlib.pyplot as plt
import numpy as np
import re

# GTEST_OK_LINE_PARSE = re.compile(r'\[ *FAILED *\] ([^. ]+)\.([^. ]+) \(([0-9]+) ms\)')
GTEST_OK_LINE_PARSE = re.compile(r'\[ *OK *\] ([a-zA-Z]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)\.([^. ]+) \(([0-9]+) ms\)')


def parse_gtest_line(line):
  parsed = GTEST_OK_LINE_PARSE.findall(line)
  return parsed[0] if len(parsed) else None

def parse_gtest_output(output):
  for line in output:
    test_run_metadata = parse_gtest_line(line)
    if test_run_metadata: yield {
      'problem': test_run_metadata[0],
      'tree_width': test_run_metadata[1],
      'max_weight': test_run_metadata[2],
      'bags_gen_type': test_run_metadata[3],
      'terminal_prob': test_run_metadata[4],
      'edge_count': test_run_metadata[5],
      'algorithm': test_run_metadata[6],
      'time': test_run_metadata[7]
    }

def build_test_name(rec):
  return 'tw:%s' % (rec['tree_width'],)

def generate_test_records(output):
  algorithms_names = []
  testcases = {}
  for rec in parse_gtest_output(output):
    if rec['algorithm'] not in algorithms_names:
      algorithms_names.append(rec['algorithm'])
    test_name = build_test_name(rec)
    if test_name not in testcases:
      testcases[test_name] = {
      }
    testcases[test_name][rec['algorithm']] = rec['time']
  return algorithms_names, testcases

def build_test_info_metadata(output):
  algorithms_names, testcases = generate_test_records(output)
  algorithms_times = [[] for l in (algorithms_names)]
  print(algorithms_names)
  print(testcases)
  for tc in testcases:
    for (idx, alg) in enumerate(algorithms_names):
      algorithms_times[idx].append(int(testcases[tc][alg]))
  return testcases.keys(), algorithms_names, algorithms_times

def build_bar_graph(tests_cases_names, algorithms_names, values, output_file, width=0.35):
  lbl_locations = np.arange(len(tests_cases_names))
  fig, ax = plt.subplots()
  results = []
  multiplier = [-1, 1]
  for idx, alg in enumerate(algorithms_names):
    results.append(ax.bar(lbl_locations +  multiplier[idx] * width/2, values[idx], width, label=alg))
  def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
      height = rect.get_height()
      ax.annotate('{}'.format(height),
                  xy=(rect.get_x() + rect.get_width() / 2, height),
                  xytext=(0, 3),  # 3 points vertical offset
                  textcoords="offset points",
                  ha='center', va='bottom')
  ax.set_ylabel('Running time (ms)')
  ax.set_xticks(lbl_locations)
  ax.set_xticklabels(tests_cases_names)
  ax.legend()
  for bar in results:
    autolabel(bar)
  fig.tight_layout()
  print("$ Saving graph to: ", output_file)
  plt.savefig(output_file)


def get_args():
  parser = argparse.ArgumentParser(description='Build running time graphs')
  parser.add_argument('-o', dest='output',type=str, required=True, help='name of the file where the graph will be saved.')
  parser.add_argument('-i', dest='input', type=str, required=True, help='name of the input file with gtest output')
  return parser.parse_args()

if __name__ == '__main__':
  args = get_args()
  input_file = args.input
  with open(input_file, 'r') as test_out:
    build_bar_graph(*build_test_info_metadata(test_out), output_file=args.output)