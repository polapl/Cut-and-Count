# Cut-and-Count

## Przygotowanie środowiska

```
$ mkdir build
$ pushd build
$ cmake ../
```

## Generowanie pracy w formacie PDF

```
$ make generate_pdf
```

## Uruchomienie testów

```
$ make && ./example_test
```

---

### Generowanie plików dot reprezentujących grafy w testach przykładowych

```
$ cmake -DGENERATE_DOT=ON ../
$ make && ./example_test
```
