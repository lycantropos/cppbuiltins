from hypothesis import given

from tests.utils import AlternativeNativeListsPair
from . import strategies


@given(strategies.lists_pairs)
def test_basic(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    assert bool(alternative) is bool(native)
