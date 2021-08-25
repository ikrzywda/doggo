#!/bin/python

import sys
import csv
import os
from http.server import HTTPServer, CGIHTTPRequestHandler

def results_to_html_list(results_file):
    html_list = '<ul>\r\n\t'
    with open(results_file) as results:
        results_csv = csv.reader(results)
        for rec in results_csv:
            html_list += ('<li> ' + rec[0] + ': ' + rec[1] 
                    + ' hours and ' + rec[2] + ' minutes ' + '</li>\r\n\t')
    html_list += '\r</ul>\r\n'
    return html_list

def insert_results_to_template(template_file, results, output):
    with open(output, 'w') as output_file:
        with open(template_file, 'r') as template:
            for line in template:
                if line == '<!--START-->\n':
                    output_file.write(results)
                else:
                    output_file.write(line)

def run_server():
    os.chdir('./leaderboards')
    print('Serving at localhost/80')
    localhost = HTTPServer(server_address=('', 80),
            RequestHandlerClass=CGIHTTPRequestHandler)
    localhost.serve_forever()


if __name__ == '__main__':
    #results = results_to_html_list(sys.argv[1])
    #insert_results_to_template(sys.argv[2], results, 'inserted_values.html')
    run_server()
