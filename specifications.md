PAML Specifications
--------------------------------------

PAML (Pixel Art Markup Language) is a pseudo-XML compliant form of structured data. The are some variations from traditional XML that are non-breaking allowing PAML to be parsed by modern XML parsers. This means PAML is still subject to some XML rules, such as correctly using the ampersand escape entities, etc. That said, there are some similarities with the INI configuration file format.

## Tags
There are 4 required tags:
- `<paml>`
- `<info>`
- `<defcolor>`
- `<drawpixels>`

The `<paml>` tag is used to enclose the all other tags and their associated information. It delimits when the PAML file starts and when it ends (`</paml>`). It should specify version and content information in the following format:  
```xml
<paml version="1.0.0.0" encoding="UTF-8" type="strict">`  
```
where `type` can be either `loose`, `strict` or `experimental`. Version will be following [SemVer](https://semver.org/) versioning whereas `UTF-8` is recommended for the `encoding`.
  
The `<info>` tag is used to define metadata, options and the required information to correctly draw out the pixels later on. The properties are (defined in "INI" style with a `=` between the key and the value):
- `author`: optional, used for name of author, attribution or copyright holder(s) of the artwork.
- `title`: optional, used to give a name to the artwork or the file name or some short description.
- `dateofcreation`: optional, a date as to when the artwork was created or copyright years, etc.
- `license`: optional, required if some artwork is copyrighted or trademarked.
- `xpixels`: required, specifies the width (in pixels) of the artwork.
- `ypixels`: required, specifies the height (in pixels) of the artwork.
- `sizexpixels`: optional, define the width (in pixels) of artwork's pixel, used for scaling, default is 1px.
- `sizeypixels`: optional, define the height (in pixels) of artwork's pixel, used for scaling, default is 1px.
- `bgcolor`: optional, defines the background color of the image, when each transparent pixel will be replaced with.

The `<defcolor>` tag is used to define the symbols and associated colors used in the `<drawpixels>` tag. Each color used in the palette should be defined in the following form: `symbol=colorcode`. The `symbol` used can be single or multi-character of which are valid for xml (except for `=`). That said, it is recommended to use a single character or each different color if the palette is not execessively huge. This allows for the `<drawpixels>` to have an "ASCII art" look. The `colorcode` can be HTML named colours such `DarkGoldenRod` (equivalent to `#B8860B`), hexadecimal RGB in the form `#ffffff`. Note `transparent` can be specified in the same form as an HTML named color allow for transparent pixels. That said, a predefined color is available as ' ' which is replaced by the color of `bgcolor` if specified.

The `<drawpixels>` tag is used to layout the "pixel table" indicating what pixels should be a certain color according to the palette defined in `<defcolor>`. Each sequential pixel or symbol is to be separated by a `,` (comma), new lines are not needed but offer editing visual benefits. Whitespaces are non-specific too, meaning ` , , ,` for three successive transparent pixels is the same as specifying `,,,`.

## Comments
PAML allows for line comments using `;`. PAML files are preprocess before being parsed by an xml parser allowing for things like `&` to be used when commenting. That said XML-style comments (`<!-- XML comment -->`) should also be valid but are not explicitly supported.

## Miscellaneous, Whitespace, Encoding
All leading and trailing whitespaces are trimmed. New lines are important, `CR`, `LF`, `CR+LF` should all be explicitly supported. Unless the XML parser allows for a BOM, this is not needed or explicitly supported. Character encoding should be as per XML specifications. It is recommended UTF-8 or ASCII when possible. Other encodings are not recommended.
