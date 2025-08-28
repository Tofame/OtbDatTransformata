**Disclaimer: Unless otherwise stated, all rights to this sources are reserved by the author. This software may not be copied, modified (except via a pull request to this repository), or sold - either in its original or edited form — without the express written permission of the author.**

Requires ``items.otb`` and ``Tibia.dat`` in ``data/items/`` (folder needs to be created in 'working directory' - settable in project settings of a compiler, or next to .exe if using a build)

Succesfully loads ``.dat`` and ``.otb`` of Tibia protocol 10.98, and this project's purpose is to showcase the way to do it.

Additional feature that has been added is that the program can now convert old style ``Tibia.dat`` to a new one, ordered by server id's. Such action is irreversible (advised backup),
however it's a single-time operation, and after that `.otb` format becomes obsolete, since new 'client ids' in the new `.dat`, will be matching ids previously known as server ids.
Since sid==cid... that means we just have a single id now :)


***================================================================================***

***Notes from comparing .dat and .otb, that hopefully will lead to removal of .otb***
`.otb` format is deemed to not be implementing anything new, it's just a bridge for a unnecessary convention of client-id and server-id which was allegedly created because of some weird
item removal YEARS ago. `.otb` does not contain anything additional other than *alwaysOnTopOrder=ITEM_ATTR_TOPORDER*, which is calculated anyway based on `type` from .dat.

The `.dat` code in question related to stack order:
```
                            case ItemFlag.Ground:
                                item.Type = ServerItemType.Ground;
                                item.GroundSpeed = reader.ReadUInt16();
                                break;

                            case ItemFlag.GroundBorder:
                                item.HasStackOrder = true;
                                item.StackOrder = TileStackOrder.Border;
                                break;

                            case ItemFlag.OnBottom:
                                item.HasStackOrder = true;
                                item.StackOrder = TileStackOrder.Bottom;
                                break;

                            case ItemFlag.OnTop:
                                item.HasStackOrder = true;
                                item.StackOrder = TileStackOrder.Top;
                                break;
```

Analysis on other attributes of .otb that were suspected to be custom, but are not:
- WareID - matches with `Trade as` from .dat's market.
- Type - ground, podium etc. - taken from .dat
- Name - the name is market.name from .dat
- Classification - deprecated
Description - deprecated
..many others such as stats like armor etc. deprecated in 7.x iirc
I suspect a lot where from times, when OTs didn't have items.xml, and stored attr such as `attack`, `armor`, etc. directly in .otb.
- Spritehash - doesn't appear anywhere other than OTB, unused by rme, otc, tfs.
I think its only for item editor to know recent changes and display red square in sprite window, to let user know that sprite of the item has been modified.
