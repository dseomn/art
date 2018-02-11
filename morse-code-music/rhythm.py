import sys

alphabet = {
  'a'.casefold(): '.-',
  'b'.casefold(): '-...',
  'c'.casefold(): '-.-.',
  'd'.casefold(): '-..',
  'e'.casefold(): '.',
  'f'.casefold(): '..-.',
  'g'.casefold(): '--.',
  'h'.casefold(): '....',
  'i'.casefold(): '..',
  'j'.casefold(): '.---',
  'k'.casefold(): '-.-',
  'l'.casefold(): '.-..',
  'm'.casefold(): '--',
  'n'.casefold(): '-.',
  'o'.casefold(): '---',
  'p'.casefold(): '.--.',
  'q'.casefold(): '--.-',
  'r'.casefold(): '.-.',
  's'.casefold(): '...',
  't'.casefold(): '-',
  'u'.casefold(): '..-',
  'v'.casefold(): '...-',
  'w'.casefold(): '.--',
  'x'.casefold(): '-..-',
  'y'.casefold(): '-.--',
  'z'.casefold(): '--..',
  '1'.casefold(): '.----',
  '2'.casefold(): '..---',
  '3'.casefold(): '...--',
  '4'.casefold(): '....-',
  '5'.casefold(): '.....',
  '6'.casefold(): '-....',
  '7'.casefold(): '--...',
  '8'.casefold(): '---..',
  '9'.casefold(): '----.',
  '0'.casefold(): '-----',
  }

messages = [
  # Primarily flute and/or clarinet. Trumpet and violin occasionally, but not
  # as prominent as flute/clarinet. Various combinations of accompaniment.
  '[KA]',
  '',

  # Introduce trumpet, conversation with violin. Bassoon/cello/others riffing
  # in background.
  '[KA]',
  'Hello World STOP', # by trumpet
  'This is music K',
  'Are you sure this is music QUERY', # by violin
  'It sounds like Morse Code K',
  'Yes K',
  'Yes QUERY',
  'Yes what QUERY K',
  'Yes it is music and yes it is Morse Code K',
  'Weirdo [AR]',
  '',

  # Various instruments introducing themselves with [KA], throwing the two
  # messages back and forth and all around, and each closing with [AR].
  # Percussion filling gaps between main intruments? Low drone from organ,
  # bagpipe, or other?
  '[KA]',
  'Why K',
  'Why not K',
  '[AR]',
  '',

  # Flute and/or clarinet, with minimal/no accompaniment. Repeat middle
  # messages in various orders.
  '[KA]',
  'Was there any point to all of this QUERY',
  'Any point QUERY',
  'Was there QUERY',
  'NIL [AR]',
  ]

for msg in messages:
  print(msg)

  space_between_letters = True
  first_letter_of_word = True
  measure_lengths = [0]
  for c in msg:
    if c == '[':
      assert space_between_letters
      space_between_letters = False
      continue
    elif c == ']':
      assert not space_between_letters
      space_between_letters = True
      continue
    elif c == ' ':
      assert not first_letter_of_word
      assert space_between_letters
      sys.stdout.write('| z7 | ')
      first_letter_of_word = True
      measure_lengths += [7, 0]
      continue

    if not first_letter_of_word:
      if space_between_letters:
        sys.stdout.write('| z3 | ')
        measure_lengths += [3, 0]
      else:
        sys.stdout.write('z ')
        measure_lengths[-1] += 1

    first_mark_of_letter = True
    for mark in alphabet[c.casefold()]:
      if not first_mark_of_letter:
        sys.stdout.write('z ')
        measure_lengths[-1] += 1

      if mark == '.':
        sys.stdout.write('C ')
        measure_lengths[-1] += 1
      elif mark == '-':
        sys.stdout.write('C3 ')
        measure_lengths[-1] += 3
      else:
        raise ValueError('Invalid mark: ' + mark)

      first_mark_of_letter = False

    first_letter_of_word = False

  sys.stdout.write(
      '|\n%s |\n\n' %
          ' | '.join(['z%d' % l for l in measure_lengths if l != 0]))
