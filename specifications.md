PAML Specifications
--------------------------------------

PAML (Pixel Art Markup Language) is a pseudo-XML compliant form of structured data. The are some variations from traditional XML that are non-breaking allowing PAML to be parsed by modern XML parsers. This means PAML is still subject to some XML rules, such as correctly using the ampersand escape entities, etc. That said, there are some similarities with the INI configuration file format.

## Tags
There are 4 required tags:
- `<paml>`
- `<info>`
- `<defcolor>`
- `<drawpixels>`

The `<paml>` tag is used to enclose the all other tags and their associated information. It delimits when the PAML file starts and when it ends (`</paml>`).
  
The `<info>` tag is used to define metadata, options and the required information to correctly draw out the pixels later on. The properties are (defined in "INI" style with a `=` between the key and the value):
- `author`: optional, used for name of author, attribution or copyright holder(s) of the artwork.
- `dateofcreation`: optional, a date as to when the artwork was created or copyright years, etc.
- `license`: optional, required if some artwork is copyrighted or trademarked.
- `xpixels`: required, specifies the width (in pixels) of the artwork
- `ypixels`: required, specifies the height (in pixels) of the artwork
- `sizexpixels`: optional, define the width (in pixels) of artwork's pixel, used for scaling, default is 1px
- `sizeypixels`: optional, define the height (in pixels) of artwork's pixel, used for scaling, default is 1px
- `bgcolor`: optional, defines the background color of the image, when each transparent pixel will be replaced with

The `<defcolor>` tag is used to define the symbols and associated colors used in the `<drawpixels>` tag. 

The `<drawpixels>` tag

## Comments
PAML allows for line comments using `;`. PAML files are preprocess before being parsed by an xml parser allowing for things like `&` to be used when commenting. That said XML-style comments (`<!-- XML comment -->`) should also be valid but are not explicitly supported.

