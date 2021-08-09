from hypothesis import given

from tests.utils import AlternativeNativeIntsPair
from . import strategies


@given(strategies.ints_pairs)
def test_basic(pair: AlternativeNativeIntsPair) -> None:
    alternative, native = pair

    assert str(alternative) == str(native)
