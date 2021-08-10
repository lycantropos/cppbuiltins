from hypothesis import strategies

from tests.utils import to_alternative_native_ints_pair

ints_pairs = strategies.integers().map(to_alternative_native_ints_pair)
