import mido
import argparse

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
    parser = argparse.ArgumentParser("\nget track notes data to a lua file\n--input input_file\n--output output_file\n--track track_index")
    parser.add_argument('--input', type=str, help='input midi file')
    parser.add_argument('--output', type=str, help='output data file')
    parser.add_argument('--track', type=int, help='track index')
    args = parser.parse_args()
    if args.input == None or args.output == None:
        print("please set input and output")
        exit()
    if args.track is None:
        args.track = 0
    main(args.input, args.output, args.track)
