import random

# Notes to use in a melody, (main note, is sharp, treble clef).
notes = [
  ("C,", False, False),
  ("C,", True, False),
  ("D,", False, False),
  ("D,", True, False),
  ("E,", False, False),
  ("F,", False, False),
  ("F,", True, False),
  ("G,", False, False),
  ("G,", True, False),
  ("A,", False, False),
  ("A,", True, False),
  ("B,", False, False),

  ("C", False, True),
  ("C", True, True),
  ("D", False, True),
  ("D", True, True),
  ("E", False, True),
  ("F", False, True),
  ("F", True, True),
  ("G", False, True),
  ("G", True, True),
  ("A", False, True),
  ("A", True, True),
  ("B", False, True),

  ("c", False, True),
  ("c", True, True),
  ("d", False, True),
  ("d", True, True),
  ("e", False, True),
  ("f", False, True),
  ("f", True, True),
  ("g", False, True),
  ("g", True, True),
  ("a", False, True),
  ("a", True, True),
  ("b", False, True),
  ]

def note_str(measure):
  """Given a list of notes in the same measure, return the strings to represent
  them."""

  is_sharp = {note[0]: False for note in notes}

  ret = []
  for note in measure:
    if is_sharp[note[0]] == note[1]:
      ret.append(note[0])
    elif note[1]:
      ret.append('^' + note[0])
    else:
      ret.append('=' + note[0])
    is_sharp[note[0]] = note[1]

  return ret


print("#3")
r = random.Random(3)
for measure in range(99):
  note_idx = r.randint(0, len(notes) - 2)
  note_strs = note_str([notes[note_idx], notes[note_idx+1]])
  if notes[note_idx][2] and notes[note_idx+1][2]:
    print("[V:T] %s %s z |\\" % (note_strs[0], note_strs[1]))
    print("[V:B] Z |\\")
  elif not notes[note_idx][2] and not notes[note_idx+1][2]:
    print("[V:T] Z |\\")
    print("[V:B] %s %s z |\\" % (note_strs[0], note_strs[1]))
  else:
    print("[V:T] %s %s z |\\" % (
        note_strs[0] if notes[note_idx][2] else 'z',
        note_strs[1] if notes[note_idx+1][2] else 'z',
        ))
    print("[V:B] %s %s z |\\" % (
        note_strs[0] if not notes[note_idx][2] else 'z',
        note_strs[1] if not notes[note_idx+1][2] else 'z',
        ))
print()
