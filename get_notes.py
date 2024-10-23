import mido
import argparse
from mido import MidiFile
import pygame
import pygame.midi
import time

def play_track(midi_file_path, track_index):
    # init pygame
    pygame.init()
    # init pygame.midi
    pygame.midi.init()
    mid = MidiFile(midi_file_path)

    if track_index >= len(mid.tracks):
        print("invalid track")
        return
    # default tempo
    tempo = 500000  # 120 BPM
    for msg in mid.tracks[0]:
        if msg.type == 'set_tempo':
            tempo = msg.tempo
            print(f"tempo is set {tempo} in midi file {midi_file_path} track[0]")
    microseconds_per_beat = tempo
    seconds_per_beat = microseconds_per_beat / 1_000_000.0
    ticks_per_beat = mid.ticks_per_beat

    output = pygame.midi.Output(0)

    for msg in mid.tracks[track_index]:
        if not msg.is_meta:
            if msg.type == 'note_on':
                output.note_on(msg.note, msg.velocity)
            #elif msg.type == 'note_off':
            #    output.note_off(msg.note, msg.velocity)
            time_in_seconds = (msg.time / ticks_per_beat) * seconds_per_beat
            time.sleep(time_in_seconds)
    output.close()
    pygame.midi.quit()
    pygame.quit()

# midi file of track_index data to list
def midi_to_list(midi_file_path, track_index):
    mid = mido.MidiFile(midi_file_path)
    midi_values = []
    velocities = []
    for i, track in enumerate(mid.tracks):
        if i == track_index:
            for msg in track:
                if msg.type == 'note_on' and msg.velocity > 0:
                    midi_values.append(msg.note)
                    velocities.append(msg.velocity)
    return midi_values, velocities

# save value list to a lua file
def save_value(file_path, values):
    with open(file_path, "w") as f:
        lv = len(values)
        f.write("return {\n")
        for i, v in enumerate(values):
            if i == lv - 1:
                f.write(f"{v}")
            else:
                f.write(f"{v}, ")
        f.write("\n}")
        f.close()
        print(f"writing data to {file_path} done!")

# main function
def main(input, output, track):
    values, _ = midi_to_list(input, track)
    save_value(output, values)

if __name__ == '__main__':
    parser = argparse.ArgumentParser("\nget track notes data to a lua file")
    parser.add_argument('--input', type=str, help='str, input midi file')
    parser.add_argument('--output', type=str, help='str, output data file')
    parser.add_argument('--track', type=int, help='int, track index')
    parser.add_argument('--play', type=bool, help='bool, if to play a track')
    args = parser.parse_args()
    if args.input == None:
        print("please set input")
        exit()
    if args.track is None:
        args.track = 0
    if args.play == None and args.output != None:
        print(f"now to convert data from {args.input}, track {args.track}")
        main(args.input, args.output, args.track)
    elif args.play:
        print(f"now to play {args.input}, track {args.track}")
        play_track(args.input, args.track)

