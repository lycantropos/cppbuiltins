import re

from hypothesis import strategies

from tests.utils import AlternativeInt

whitespaces_class = r'[\f\n\r\t\v ]'
decimal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*\d(_?\d+)*{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
binary_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[0-1](_?[0-1]+)*{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
octal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[0-7](_?[0-7]+)*{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
hexadecimal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[\da-f](_?[\da-f]+)*{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
prefixed_binary_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*0b(_?[0-1]+)+{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
prefixed_octal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*0o(_?[0-7]+)+{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
prefixed_hexadecimal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*0x(_?[\da-f]+)+{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))

ints = (strategies.builds(AlternativeInt, decimal_int_strings)
        | strategies.builds(AlternativeInt, decimal_int_strings,
                            strategies.sampled_from([0, 10]))
        | strategies.builds(AlternativeInt, binary_int_strings,
                            strategies.just(2))
        | strategies.builds(AlternativeInt, hexadecimal_int_strings,
                            strategies.just(16))
        | strategies.builds(AlternativeInt, octal_int_strings,
                            strategies.just(8))
        | strategies.builds(AlternativeInt, prefixed_binary_int_strings,
                            strategies.sampled_from([0, 2]))
        | strategies.builds(AlternativeInt, prefixed_hexadecimal_int_strings,
                            strategies.sampled_from([0, 16]))
        | strategies.builds(AlternativeInt, prefixed_octal_int_strings,
                            strategies.sampled_from([0, 8])))
