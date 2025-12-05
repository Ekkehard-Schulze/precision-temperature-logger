#!/usr/bin/env python3

# pylint: disable=trailing-whitespace
# pylint: disable=line-too-long
# pylint: disable=invalid-name
# pylint: disable=too-many-lines
# pylint: disable=too-many-public-methods
# pylint: disable=import-error
# pylint: disable=missing-function-docstring
# pylint: disable=missing-class-docstring
# pylint: disable=consider-using-f-string
# pylint: disable=import-outside-toplevel
# pylint: disable=unspecified-encoding

# flake8 --ignore=F541,E501 "plots_and_statistics_of_time_series.py"

r'''

Plotly time series data with ISO date format

Lines with wrongly formatted ISO dates are optionally removed by parser

Check user settings below to control tasks performed by this code.

'''

import re
import os
import sys
import datetime
import pathlib
import platform
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# ----------- user settings: selct tasks to be done using True/False ----------------------

DO_interactive_browser_plot = True
SAVE_interactive_html_plot = True
SAVE_statistics_file = True
WRITE_timeline_png = False

CHAR_CODING = "utf8"


# ----------------- content definitions ------------------------

STATS_file_separator = "\t"

parse_for_bad_lines = True
parse_for_bad_datelines = True

allsensor_options = ['TMP1', 'TMP2', 'TMP3', 'TMP4']

BAT_voltage_field_name = 'Vbatt'

Date_time_field_name = "Date_time"

LOGGER_ID_field_name = "Logger-id"

default_separator = "\t"

ISO_DATE_TIME_format = "%Y-%m-%dT%H:%M:%S"

default_title = "TSV log data"

# -----------------end of  content definitions ------------------------

SECONDS_PER_HOURS = 60 * 60  # 3600 führt zu Rundungsfehlern, obwohl beide integer sind!

if SAVE_interactive_html_plot:
    import plotly.offline


def add_prefix_to_file_stem(ppath, stem_prefixl):
    '''inserts a prefix into a path at the beginning of the file name'''
    return(
        str(pathlib.Path(ppath).parent)
        + os.sep
        + stem_prefixl
        + str(pathlib.Path(ppath).stem)
        + str(pathlib.Path(ppath).suffix)
    )


def write_statistics_file(dfl, logger_tsv_filel, accepted_sensorsl, skip_frames=0):
    '''write statistics file'''
    n_txt = "{number:.2f}"
    stat_header_line = ("Sensor" + STATS_file_separator
        + "Mean [°C]" + STATS_file_separator
        + "Min [°C]" + STATS_file_separator
        + "Max [°C]" + STATS_file_separator
        + "Stdev [°C]" + STATS_file_separator
        + "duration" + STATS_file_separator
        #  + "2 hrs equilibration omitted"
        + "\n"
    )

    starttime = datetime.datetime.strptime(dfl[Date_time_field_name].iloc[0], ISO_DATE_TIME_format)
    endtime = datetime.datetime.strptime(dfl[Date_time_field_name].iloc[-1], ISO_DATE_TIME_format)
    timeDelta = endtime - starttime

    stat_data_lines = []
    for sensor in accepted_sensorsl:  # find and plot first track
        try:
            if sensor in dfl.columns:
                stat_line = (
                    sensor                                                  + STATS_file_separator
                    + n_txt.format(number=dfl[sensor][skip_frames:].mean()) + STATS_file_separator
                    + n_txt.format(number=dfl[sensor][skip_frames:].min())  + STATS_file_separator
                    + n_txt.format(number=dfl[sensor][skip_frames:].max())  + STATS_file_separator
                    + n_txt.format(number=dfl[sensor][skip_frames:].std())  + STATS_file_separator
                    # + str(datetime.timedelta(seconds = int(timeDelta))) + STATS_file_separator
                    + str(timeDelta) + STATS_file_separator
                )
                # if skip_frames >0:
                # stat_line += 'yes'
                # else:
                # stat_line += 'no'
                stat_data_lines.append(stat_line + '\n')
        except:
            print('...statistics for sensor ' + sensor + ' failed')

    last_datetime = str(dfl[Date_time_field_name].iloc[-1]).replace(":", "_")
    prefix = last_datetime + "_statistics_"
    sfile_name = add_prefix_to_file_stem(logger_tsv_filel, prefix)

    if stat_data_lines:
        try:
            with open(sfile_name, "w") as log_file:
                log_file.write('\n ' + stat_header_line + ' ' + ' '.join(stat_data_lines) + '\n')
        except PermissionError:
            print('Failed to open statistics file.\nClose other application (e.g. Excel) blocking ' + logger_tsv_filel + '\n')
            input('Press Enter to quit.')
            sys.exit()


def bad_date_and_repeated_header_masker(linesl):
    '''masks badlyformatted iso dates. Used for my circuit-python logger'''
    date_pattern_c = re.compile(r'\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d')
    skiplinesl = []
    toplines = True
    check_datetime = Date_time_field_name in linesl[0]  # bool
    for ln, l in enumerate(linesl):
        if l.startswith('#'):  # comment found, redundant with pandas df load comment =
            skiplinesl.append(ln)
        elif LOGGER_ID_field_name in l:
            if toplines:
                if ln > 0:
                    skiplinesl.append(ln - 1)  # leave last of multiple top header lines. This is required for re-starts in case the restart was done due to an observed sensor count failure (LED blinks)
            else:
                skiplinesl.append(ln)
        else:
            toplines = False  # first values found
            if check_datetime and parse_for_bad_datelines:
                if not re.search(date_pattern_c, l):
                    skiplinesl.append(ln)
    return skiplinesl


def main():

    print("initializing...")

    # -------------------- file picker ------------------------
    if (len(sys.argv) < 2 or len(sys.argv) > 2):  # 2 is for one parameter , 1 means no argument given, 2 means one argument given
        from tkinter import Tk
        from tkinter.filedialog import askopenfilename
        Tk().withdraw()                      # we don't want a full GUI, so keep the root window from appearing
        logger_tsv_file = askopenfilename(initialdir=os.getcwd(), title="Select temperature logger .tsv file. Use -h for help in console mode.", filetypes=(("tsv files", "*.tsv"), ("csv files", "*.csv"), ("all files", "*.*")))  # show an "Open" dialog box and return the path to the selected file
        if logger_tsv_file == "":
            sys.exit()  # Abort has been pressed
        # logger_tsv_file = logger_tsv_file.split('/')[-1]
        if platform.system() == 'Windows':
            logger_tsv_file = logger_tsv_file.replace("/", os.sep)

    else:
        if sys.argv[1] == '-h' or sys.argv[1] == '--help':
            print(__doc__)
            sys.exit()
        else:
            logger_tsv_file = sys.argv[1]

    # -------------------- read data and convert unix date   ----------------------------
    # ----------------- find bad line numbers, such as repeated headers from logger re-starts
    print("loading...")
    skiplines = []

    with open(logger_tsv_file, encoding='utf-8-sig', errors="ignore") as f:
        lines = f.readlines()

    skiplines = bad_date_and_repeated_header_masker(lines)

    df = pd.read_csv(logger_tsv_file, encoding=CHAR_CODING, sep=None, comment='#', engine='python', skiprows=skiplines, on_bad_lines="skip")

    accepted_sensors = allsensor_options

    if Date_time_field_name not in df.columns:
        print('Datum stamp "' + Date_time_field_name + '" is missing in tsv-data')
        print(df)
        sys.exit()

    my_title = df[LOGGER_ID_field_name][1] if LOGGER_ID_field_name in df.columns else default_title

    if SAVE_statistics_file:
        print("Writing statistics file...")
        write_statistics_file(df, logger_tsv_file, accepted_sensors)

    if DO_interactive_browser_plot or SAVE_interactive_html_plot or WRITE_timeline_png:

        # ---------------------- plot battery voltage -----------------------------------------

        if BAT_voltage_field_name in df.columns and ('unix-time' in df.columns or Date_time_field_name in df.columns):

            timefield = Date_time_field_name if Date_time_field_name in df.columns else 'unix-time'

            print(f"plotting {BAT_voltage_field_name}...")

            fig_bat = go.Figure(go.Scatter(x=df[timefield], y=df[BAT_voltage_field_name], name=BAT_voltage_field_name))

            fig_bat.update_layout(title=my_title + ": Battery Voltage [mV]    (full: 4200, empty: 3600)", plot_bgcolor='rgb(230, 230,230)', showlegend=True)

            # fig_bat.show()

        # -----------plot temperature data violin plot.

        try:   # just in case something happens, we would still get the timeline plot
            print("plotting violin plot...")

            n = 0
            for sensor in accepted_sensors:
                if sensor in df.columns:
                    n += 1
            fig_violine = make_subplots(rows=1, cols=n)

            c = 0
            for sensor in accepted_sensors:

                if sensor in df.columns:
                    c += 1
                    fig_violine.append_trace(
                        go.Violin(
                            y=df[sensor],
                            name=sensor,
                            box_visible=True,
                            # line_color="black",
                            meanline_visible=True,
                        ), row=1, col=c
                    )  # plot second to last trace

            # fig_violine.update_layout(title='temperature logged',plot_bgcolor='rgb(230, 230,230)', showlegend=True)
            # fig_violine.update_layout(title=df[LOGGER_ID_field_name][1],plot_bgcolor='rgb(230, 230,230)', showlegend=True)

            # fig_violine.show()

        except:
            print('Violin plot failed')
            raise

        # -------------------- plot temperature data timeline plot ----------------------------

        print("plotting timeline...")
        # Create figure with secondary y-axis

        fig_time = make_subplots(specs=[[{"secondary_y": True}]])
        for sensor in accepted_sensors:
            # find and plot first track
            if sensor in df.columns:
                # fig_time.add_trace(go.Scatter(x = df[Date_time_field_name], y = df[sensor], name=sensor, hovertemplate="%{x|%d %b %Y, %H:%M:%S} %{y}"), secondary_y=False)
                fig_time.add_trace(go.Scatter(x=df[Date_time_field_name], y=df[sensor], name=sensor, hovertemplate="%{x|%d %b %Y, %H:%M} %{y}"), secondary_y=False)

        fig_time.update_layout(title=my_title, plot_bgcolor='rgb(230, 230,230)', showlegend=True)

        # all fig calculated, now generate output
        if SAVE_interactive_html_plot:
            print("Writing html...")
            last_datetime = str(df[Date_time_field_name].iloc[-1]).replace(":", "_")
            stem_prefix = last_datetime + "_interactive_plot_"
            f_html_name = str(pathlib.Path(logger_tsv_file).parent) + os.sep + stem_prefix + str(pathlib.Path(logger_tsv_file).stem) + ".html"
            plotly.offline.plot(fig_time, filename=f_html_name, auto_open=False)

        if DO_interactive_browser_plot:
            fig_bat.show()
            fig_violine.show()
            fig_time.show()
            
        if WRITE_timeline_png:
            print("Writing png...")
            last_datetime = str(df[Date_time_field_name].iloc[-1]).replace(":", "_")
            stem_prefix = last_datetime + "_"
            png_name = str(pathlib.Path(logger_tsv_file).parent) + os.sep + stem_prefix + str(pathlib.Path(logger_tsv_file).stem) + ".png"
            width_l=2880
            height_l=1620
            my_ytick_font_size = 33                                  # my default is 20 for sparse plots, use 16 for very many plots
            my_xtick_font_size = 33                                   # my default is 20 for sparse plots, use 16 for very many plots
            # my_yaxes_title_font_size = 20           
            fig_time.update_layout(xaxis=dict(tickfont = dict(size=my_xtick_font_size)), yaxis=dict(tickfont = dict(size=my_ytick_font_size)) )            
            fig_time.update_layout(width=width_l, height=height_l)
            #fig_time.update_layout(font=dict(family='Times New Roman',size=20,title=dict(font=dict(family='Ariel', size=33))))            
            fig_time.update_traces(marker={'size': 15})
            fig_time.update_layout(title_font_size=33)            
            fig_time.update_layout(legend_font_size=33)
            fig_time.update_yaxes(automargin=True, title_standoff = 33)            
            fig_time.write_image(png_name)             

if __name__ == '__main__':
    main()
