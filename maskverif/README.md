# Formal $t$-(S)NI Verification
This directory contains the first- and second-order description of the gadgets proposed in Section 4 and 5, which can be used to formally verify the claimed security notions using maskVerif. Due to the tool's (computational) limitations, we limit the scope to first- and second-order verification of the (S)NI properties.

## :clipboard: Requirements
### [maskVerif](https://gitlab.com/benjgregoire/maskverif)
Install the maskVerif tool through:
```shell
git clone https://gitlab.com/benjgregoire/maskverif.git
```
Open the nix shell with required packages:
```shell
nix-shell -p ocamlPackages.menhirLib ocamlPackages.ocamlbuild ocamlPackages.findlib ocamlPackages.zarith ocamlPackages.ocamlgraph ocaml ocamlPackages.menhir ocamlPackages.batteries
```
Make the maskVerif tool:
```shell
eval $(opam env)
```
```shell
make
```
## :hammer_and_wrench: Running the script
Finally, run the script:
```shell
./maskverif <mUOV.mv
```
