#! /usr/bin/env python3

import numpy as np
import scipy.stats as stats
import pandas as pd
import matplotlib.pyplot as plt
import yaml 
import sys
from docopt import docopt 
from prettytable import PrettyTable

SAMPLING_TIME_SECS = 5
SEC_TO_HRS = 1 / 3600
SEC_TO_MINS = 1 / 60

def read_sensor_data(file_name):
    """
    Read sensor data from a yaml file

    Args:
        file_name: str, name of the yaml file
    
    Returns:
        temperature: list, temperature values
        humidity: list, humidity values
    """
    with open(file_name, 'r') as f:
        data = yaml.load(f, Loader=yaml.FullLoader)
     
    temperature = [value[0] for value in data['data']]
    humidity = [value[1] for value in data['data']]
    return temperature, humidity

def plot_sensor_data(temperature, humidity, temperature_metrics, humidity_metrics, *, size = 16, sampling_time = 5, time_unit='secs'):
    """
    Plot sensor data

    Args:
        temperature: list, temperature values
        humidity: list, humidity values
    """
    def subplot(axes, row, col, data, title, ylabel, xlabel, color):
        time = [i * sampling_time for i in range(len(data))]
        axes[row, col].plot(time, data, color)
        axes[row, col].set_title(title)
        axes[row, col].set_ylabel(ylabel)
        axes[row, col].set_xlabel(xlabel)
    
    def hist(axes, row, col, data, title, ylabel, xlabel, color):
        axes[row, col].hist(data, bins=40, color=color)
        axes[row, col].set_title(title)
        axes[row, col].set_ylabel(ylabel)
        axes[row, col].set_xlabel(xlabel)
    
    def violinplot(axes, row, col, data, title, ylabel, xlabel, color, scale=1):
        data_per_time_unit = [data[i:i + int(scale / sampling_time)] for i in range(0, len(data), int(scale / sampling_time))]
        labels = [f'{i* scale:.0f}-{(i+1)*scale:.0f}' for i in range(len(data_per_time_unit))]
        parts = axes[row, col].violinplot(data_per_time_unit, showmedians=True)
        for pc in parts['bodies']:
            pc.set_facecolor(color)
            pc.set_edgecolor(color)
            pc.set_alpha(0.5)
        axes[row, col].set_title(title)
        axes[row, col].set_ylabel(ylabel)
        axes[row, col].set_xlabel(xlabel)
        axes[row, col].set_xticks(np.arange(len(labels)), rotation=45, labels=labels)
    
    def boxplot(axes, row, col, data, title, ylabel, xlabel, color, scale=1):
        data_per_time_unit = [data[i:i + int(scale / sampling_time)] for i in range(0, len(data), int(scale / sampling_time))]
        labels = [f'{i* scale:.0f}-{(i+1)*scale:.0f}' for i in range(len(data_per_time_unit))]
        axes[row, col].boxplot(data_per_time_unit, patch_artist=True, boxprops=dict(facecolor=color), labels=labels)
        axes[row, col].set_title(title)
        axes[row, col].set_ylabel(ylabel)
        axes[row, col].set_xlabel(xlabel)
        axes[row, col].set_xticks([i + 1 for i in range(len(labels))], labels, rotation=45)
    
    def point_scatter(axes, row, col, data, *, color, label):
        axes[row, col].scatter(data[0] * sampling_time, data[1], color=color)
        axes[row, col].text(data[0], data[1],
                f'{label}: ({data[1]})',
                horizontalalignment='right', verticalalignment='top')
    
    def median_and_deviation(axes, row, col, data, metrics, color):
        axes[row, col].axhline(metrics['median'], color=color, linestyle='-', label='Median')
        axes[row, col].fill_between(range(int(np.ceil(len(data) * sampling_time + 1))), 
                        metrics['median'] - metrics['std'],
                        metrics['median'] + metrics['std'],
                        color=color, alpha=0.2, label='Standard Deviation')
        axes[row, col].legend()

    fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(size, size))
    subplot(axes, 0, 0, temperature, 'Temperature', 'Celsius[°C]', f'Time [{time_unit}]', 'darkorange')
    point_scatter(axes, 0, 0,temperature_metrics['min'], color='navy', label='Min')
    point_scatter(axes, 0, 0,temperature_metrics['max'], color='navy', label='Max')
    median_and_deviation(axes, 0, 0,temperature, temperature_metrics, 'salmon')
    hist(axes, 0, 1, temperature, 'Temperature Histogram', 'Frequency', 'Celsius[°C]', 'darkorange')
    boxplot(axes, 0, 2, temperature, 'Temperature Violinplot', 'Celsius[°C]', f'Time[{time_unit}]', 'sandybrown', scale=2)
    
    subplot(axes, 1, 0, humidity, 'Humidity', 'Percentage[%]', f'Time [{time_unit}]', 'deepskyblue')
    point_scatter(axes, 1, 0, humidity_metrics['min'], color='salmon', label='Min')
    point_scatter(axes, 1, 0, humidity_metrics['max'], color='salmon', label='Max')
    median_and_deviation(axes, 1, 0, humidity, humidity_metrics, 'skyblue')
    hist(axes, 1, 1, humidity, 'Humidity Histogram', 'Frequency', 'Percentage[%]', 'deepskyblue')
    boxplot(axes, 1, 2, humidity, 'Humidity Violinplot', 'Percentage[%]', f'Time[{time_unit}]', 'skyblue', scale=2)
    
    fig.subplots_adjust(hspace=0.35, wspace=0.25)
    plt.show()

def metrics(data):
    """
    Calculate metrics of the data

    Args:
        data: list, data values
    
    Returns:
        dict, metrics of the data
    """
    return {
        'mean': round(np.mean(data), 2),
        'std': round(np.std(data), 2),
        'median': np.median(data),
        'range': round(np.ptp(data), 2),
        'iqr': round(stats.iqr(data), 2),
        'max': (np.argmax(data), max(data)),
        'min': (np.argmin(data), min(data))
    }

def metrics_description():
    """
    Return metrics description

    Returns:
        str, metrics description
    """
    return {
        'mean': 'Average of the data',
        'std': 'Dispersion of the data',
        'median': 'Central tendency',
        'range': 'Range of the data',
        'iqr': 'Range between the 1st and 3rd quartile',
        'max': 'Maximum value',
        'min': 'Minimum value',
    }

def print_table(metrics):
    """
    Print metrics table

    Args:
        metrics: dict, metrics of the data
    """
    df = pd.DataFrame(metrics)
    pt = PrettyTable()
    pt.field_names = [item.capitalize() for item in ["metric"] + list(metrics.keys())]
    for row in df.itertuples():
        pt.add_row(row)
    pt.align = 'l'
    print(pt)


if __name__ == "__main__":
    usage = f"""
        Temperature and Humidity Data Visualizer

        Usage:
            {sys.argv[0]} <file_path>
            {sys.argv[0]} -h | --help

        Options:
            -h --help    Show this screen
    """
    args = docopt(usage, help=True)

    temperature, humidity = read_sensor_data(args['<file_path>'])
    temperature_metrics = metrics(temperature)
    humidity_metrics = metrics(humidity)

    plot_sensor_data(
        temperature,
        humidity,
        temperature_metrics,
        humidity_metrics,
        sampling_time=SAMPLING_TIME_SECS * SEC_TO_HRS,
        time_unit='hrs'
    )

    temperature_metrics['max'] = temperature_metrics['max'][1]
    temperature_metrics['min'] = temperature_metrics['min'][1]
    humidity_metrics['max'] = humidity_metrics['max'][1]
    humidity_metrics['min'] = humidity_metrics['min'][1]
    print_table({
        "temperature": temperature_metrics,
        "humidity": humidity_metrics,
        "description": metrics_description()
    })