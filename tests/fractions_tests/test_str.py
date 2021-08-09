from hypothesis import given

from tests.utils import AlternativeNativeFractionsPair
from . import strategies


@given(strategies.fractions_pairs)
def test_basic(pair: AlternativeNativeFractionsPair) -> None:
    alternative, native = pair

    assert str(alternative) == str(native)
