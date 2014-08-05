#LEGO® Enhanced Graphics Object (LEGO) File Format Specification

A file format for representing 2D LEGO® brick images

_Note: LEGO® is a trademark of the LEGO Group of companies which does not sponsor, authorize or endorse this project._

##File Header

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 4        | Signature: always equal to LEG0 (0x4c 0x45 0x47 0x30)             |
| 2        | Version (Major/Minor)                                             |
| 6        | Reserved: must be zeroes                                          |
| 2        | Width of the image in 1x1 bricks (1 to 1000)                      |
| 2        | Height of the image in 1x1 bricks (1 to 1000)                     |

##Resource Blocks

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 4        | Signature                                                         |
| 4        | Length of data                                                    |
| Variable | Resource data                                                     |
| 4        | 32-bit hash of resource data                                      |

The `signature` is a 4-byte ASCII string that denotes one of the 4 types of resource blocks below.

The `32-bit hash` is a hash of the `resource data` using an implementation of the MurmerHash function and is used to detect block-level data corruption.

###Info Block (INFO)

The INFO block holds the metadata for the LEGO image to facilitate sharing and versioning.

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 4        | Signature: INFO (0x49 0x4e 0x46 0x4f)                             |
| 4        | Length of data                                                    |
| Variable | Resource data                                                     |
| 4        | 32-bit hash of resource data                                      |

The resource data for the INFO block consists of none or many of the following sub-blocks:

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 2        | Sub-block signature (0x?? 0x??)                                   |
| 1        | Length of sub-block data                                          |
| Variable | Sub-block data (Text)                                             |

The following sub-blocks and signatures are supported:

* NM (0x4e 0x4d) - Image name
* VR (0x56 0x52) - Version
* DS (0x44 0x53) - Description
* AN (0x41 0x4e) - Author name
* AE (0x41 0x45) - Author email
* AU (0x41 0x55) - Author URL

###Bricks Block (BRKS)

The BRKS block contains the brick ID, color ID, and brick rotation for each brick used in the image. The blocks are placed in the image in the order they appear and originating from the top-most and then left-most open space.

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 4        | Signature: BRKS (0x42 0x52 0x4b 0x53)                             |
| 4        | Length of data                                                    |
| Variable | Resource data                                                     |
| 4        | 32-bit hash of resource data                                      |

The resource data for the BRKS block adhere to the following format for each individual brick:

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 1        | Brick ID                                                          |
| 1        | Color ID                                                          |
| 1        | Brick rotation (0x00: 0°, 0x01: 90°, 0x02: 180°, 0x03: 270°)      |

###New Colors Block (NCLR)

Colors may be added that don't fall within the official LEGO® color palette.

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 4        | Signature: NCLR (0x4e 0x43 0x4c 0x52)                             |
| 4        | Length of data                                                    |
| Variable | Resource data                                                     |
| 4        | 32-bit hash of resource data                                      |

The resource data for the NCLR block adheres to the following format:

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 1        | Color ID. Using an existing ID will replace the existing color    |
| 1        | Color value, Red                                                  |
| 1        | Color value, Green                                                |
| 1        | Color value, Blue                                                 |
| 1        | Length of name                                                    |
| Variable | Name                                                              |

###New Bricks Block (NBRK)

Bricks may be added that are not included in the default brick library.

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 4        | Signature: NBRK (0x4e 0x42 0x52 0x4b)                             |
| 4        | Length of data                                                    |
| Variable | Resource data                                                     |
| 4        | 32-bit hash of resource data                                      |

The resource data for the NBRK block adheres to the following format:

| Length   | Description                                                       |
| -------- | ----------------------------------------------------------------- |
| 1        | Brick ID. Using an existing ID will replace the existing brick    |
| 1        | Brick width in 1x1 bricks (1 to 255)                              |
| 1        | Brick height in 1x1 bricks (1 to 255)                             |
| 1        | Length of name                                                    |
| Variable | Name                                                              |

##Bricks

The simplest bricks found in [Pick A Brick](http://shop.lego.com/en-US/Pick-A-Brick-ByTheme) on the LEGO® shop.

| ID     | Width  | Height | Name   |
| ------ | ------ | ------ | ------ |
| 00     | 1      | 1      | 1x1    |
| 01     | 2      | 1      | 1x2    |
| 02     | 3      | 1      | 1x3    |
| 03     | 4      | 1      | 1x4    |
| 04     | 6      | 1      | 1x6    |
| 05     | 8      | 1      | 1x8    |
| 06     | 10     | 1      | 1x10   |
| 07     | 12     | 1      | 1x12   |
| 08     | 16     | 1      | 1x16   |
| 09     | 2      | 2      | 2x2    |
| 0a     | 3      | 2      | 2x3    |
| 0b     | 4      | 2      | 2x4    |
| 0c     | 6      | 2      | 2x6    |
| 0d     | 8      | 2      | 2x8    |

##Colors

The official solid, transparent, and special LEGO® colors found on [Brickipedia](http://lego.wikia.com/wiki/Colour_Palette).

| ID     | Color   | Name                                   |
| ------ | ------- | -------------------------------------- |
| 00     | #ffffff | White                                  |
| 01     | #d9bb7b | Brick-Yellow                           |
| 02     | #d67240 | Nougat                                 |
| 03     | #de000d | Bright Red                             |
| 04     | #0057a8 | Bright Blue                            |
| 05     | #fec400 | Bright Yellow                          |
| 06     | #010101 | Black                                  |
| 07     | #007b28 | Dark Green                             |
| 08     | #009624 | Bright Green                           |
| 09     | #a83d15 | Dark Orange                            |
| 0a     | #478cc6 | Medium Blue                            |
| 0b     | #e76318 | Bright Orange                          |
| 0c     | #95b90b | Bright Yellowish-Green                 |
| 0d     | #9c006b | Bright Reddish Violet                  |
| 0e     | #5e748c | Sand Blue                              |
| 0f     | #8d7452 | Sand Yellow                            |
| 10     | #002541 | Earth Blue                             |
| 11     | #003416 | Earth Green                            |
| 12     | #5f8265 | Sand Green                             |
| 13     | #80081b | Dark Red                               |
| 14     | #f49b00 | Flame Yellowish Orange                 |
| 15     | #5b1c0c | Reddish Brown                          |
| 16     | #9c9291 | Medium Stone Grey                      |
| 17     | #4c5156 | Dark Stone Grey                        |
| 18     | #e4e4da | Light Stone Grey                       |
| 19     | #87c0ea | Light Royal Blue                       |
| 1a     | #de378b | Bright Purple                          |
| 1b     | #ee9dc3 | Light Purple                           |
| 1c     | #ffff99 | Cool Yellow                            |
| 1d     | #2c1577 | Medium Lilac                           |
| 1e     | #f5c189 | Light Nougat                           |
| 1f     | #300f06 | Dark Brown                             |
| 20     | #aa7d55 | Medium Nougat                          |
| 21     | #556b2f | Olive Green                            |
| 22     | #96b93b | Medium Yellowish-Green                 |
| 23     | #ffffff | Transparent                            |
| 24     | #e02a29 | Transparent Red                        |
| 25     | #b6e0ea | Transparent Light Blue                 |
| 26     | #50b1e8 | Transparent Blue                       |
| 27     | #f9ef69 | Transparent Yellow                     |
| 28     | #e76648 | Transparent Fluorescent Reddish-Orange |
| 29     | #63b26e | Transparent Green                      |
| 2a     | #faed5b | Transparent Fluorescent Green          |
| 2b     | #a69182 | Transparent Brown                      |
| 2c     | #ee9dc3 | Transparent Medium Reddish-Violet      |
| 2d     | #9c95c7 | Transparent Bright Bluish-Violet       |
| 2e     | #cee3f6 | Transparent Fluorescent Blue           |
| 2f     | #ec760e | Transparent Bright Orange              |
| 30     | #99ff66 | Transparent Bright Green               |
| 31     | #8d9496 | Silver                                 |
| 32     | #493f3b | Metallic Dark Grey                     |
| 33     | #fefcd5 | Phosphorescent Green                   |
| 34     | #aa7f2e | Warm Gold                              |