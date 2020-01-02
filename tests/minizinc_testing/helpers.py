from . import yaml

@yaml.sequence(u"!Unordered")
class Unordered:
    """
    A list where order is not important (like a set, except elements can be repeated)

    Represented by `!Unordered` in YAML.
    """

    def __init__(self, *args):
        self.items = args

    def __iter__(self):
        return iter(self.items)

    def __eq__(self, other):
        items = list(self.items)
        try:
            for x in other:
                items.remove(x)
            return True
        except ValueError:
            return False


@yaml.scalar(u"!Approx")
class Approx:
    """
    A helper which allows for approximate comparison of floats using `!Approx`
    """

    def __init__(self, value, theshold=0.000001):
        self.value = float(value)
        self.theshold = theshold

    def __eq__(self, other):
        diff = abs(other - self.value)
        return diff <= self.theshold

    def __repr__(self):
        return "Approx({})".format(self.value)

    def get_value(self):
        return self.value


@yaml.scalar(u"!Trim")
class Trim:
    """
    A helper which allows for comparison of trimmed strings with `!Trim`
    """

    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        return self.value.strip() == other.strip()

    def __repr__(self):
        return "Strip({})".format(self.value)

    def get_value(self):
        return self.value
