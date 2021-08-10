import re
import string
import sys

from hypothesis import strategies

from tests.utils import AlternativeInt

strings = strategies.text()
invalid_bases = strategies.just(1) | strategies.integers(37, sys.maxsize)
invalid_int_strings = strategies.text(strategies.characters(
        blacklist_categories=['Nd'],
        blacklist_characters=string.ascii_letters))
separator = '_?' if sys.version_info >= (3, 6) else ''
whitespaces_class = r'[\f\n\r\t\v ]'
decimal_int_strings_with_leading_zeros = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?\d({separator}\d+)*{whitespaces}*\Z'
                   .format(separator=separator,
                           whitespaces=whitespaces_class)))
decimal_int_strings_without_leading_zeros = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?(\d|[1-9]({separator}\d+)+)'
                   r'{whitespaces}*\Z'
                   .format(separator=separator,
                           whitespaces=whitespaces_class)))
prefixed_binary_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?0b({separator}[0-1]+)+'
                   r'{whitespaces}*\Z'
                   .format(separator=separator,
                           whitespaces=whitespaces_class)))
prefixed_octal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?0o({separator}[0-7]+)+'
                   r'{whitespaces}*\Z'
                   .format(separator=separator,
                           whitespaces=whitespaces_class)))
prefixed_hexadecimal_int_strings = strategies.from_regex(
        re.compile(r'\A{whitespaces}*[+-]?0x({separator}[\da-f]+)+'
                   r'{whitespaces}*\Z'
                   .format(separator=separator,
                           whitespaces=whitespaces_class)))
int_strings_with_bases = (
    (strategies.tuples(decimal_int_strings_with_leading_zeros,
                       strategies.just(10))
     | strategies.tuples(decimal_int_strings_without_leading_zeros,
                         strategies.sampled_from([0, 10]))
     | strategies.one_of([strategies.tuples(
                    strategies.from_regex(
                            re.compile(r'\A{whitespaces}*[+-]?{digits}'
                                       r'({separator}{digits}+)*'
                                       r'{whitespaces}*\Z'
                                       .format(digits=
                                               '[0-{}]'.format(max_digit),
                                               separator=separator,
                                               whitespaces=
                                               whitespaces_class))),
                    strategies.just(max_digit + 1))
                for max_digit in range(1, 10)])
     | strategies.one_of([strategies.tuples(
                    strategies.from_regex(
                            re.compile(r'\A{whitespaces}*[+-]?{digits}'
                                       r'({separator}{digits}+)*'
                                       r'{whitespaces}*\Z'
                                       .format(digits=
                                               ('[0-9a-{max_lower}'
                                                'A-{max_upper}]'
                                                .format(max_lower=max_lower,
                                                        max_upper=
                                                        max_lower.upper())),
                                               separator=separator,
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
valid_bases = strategies.just(0) | strategies.integers(2, 36)
ints = strategies.integers().map(AlternativeInt)
unary_ints = strategies.just(1).map(AlternativeInt)
zero_ints = strategies.builds(AlternativeInt)
