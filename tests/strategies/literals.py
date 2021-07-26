import re
import string

from hypothesis import strategies

whitespaces_class = r'[\f\n\r\t\v ]'
decimal_int_strings_with_leading_zeros = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?\d(_?\d+)*{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
decimal_int_strings_without_leading_zeros = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?(\d|[1-9](_?\d+)+){whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
prefixed_binary_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?0b(_?[0-1]+)+{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
prefixed_octal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?0o(_?[0-7]+)+{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
prefixed_hexadecimal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?0x(_?[\da-f]+)+{whitespaces}*\Z'
                   .format(whitespaces=whitespaces_class)))
int_strings_with_bases = (
    (strategies.tuples(decimal_int_strings_with_leading_zeros,
                       strategies.just(10))
     | strategies.tuples(decimal_int_strings_without_leading_zeros,
                         strategies.sampled_from([0, 10]))
     | strategies.one_of([strategies.tuples(
                    strategies.from_regex(
                            re.compile(r'\A{whitespaces}*[+-]?{digits}'
                                       r'(_?{digits}+)*{whitespaces}*\Z'
                                       .format(digits=
                                               '[0-{}]'.format(max_digit),
                                               whitespaces=
                                               whitespaces_class))),
                    strategies.just(max_digit + 1))
                for max_digit in range(1, 10)])
     | strategies.one_of([strategies.tuples(
                    strategies.from_regex(
                            re.compile(r'\A{whitespaces}*[+-]?{digits}'
                                       r'(_?{digits}+)*{whitespaces}*\Z'
                                       .format(digits=
                                               ('[0-9a-{max_lower}'
                                                'A-{max_upper}]'
                                                .format(max_lower=max_lower,
                                                        max_upper=
                                                        max_lower.upper())),
                                               whitespaces=
                                               whitespaces_class))),
                    strategies.just(base))
                for base, max_lower in enumerate(string.ascii_lowercase,
                                                 start=11)])
     | strategies.tuples(prefixed_binary_int_strings,
                         strategies.sampled_from([0, 2]))
     | strategies.tuples(prefixed_hexadecimal_int_strings,
                         strategies.sampled_from([0, 16]))
     | strategies.tuples(prefixed_octal_int_strings,
                         strategies.sampled_from([0, 8]))))
