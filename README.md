# Proving-gobang

Description:
* This is a project in order find the game theoretic value of the MxNxK problem with M=N=infty, K=7.
With other words: - we want to prove (Does exists win-strategy for player one?) in the 7-in-a-row game on infinite board. 

# Install:
```
git clone https://github.com/dczifra/proving_gobang_public.git
cd proving_gobang_public
docker pull doma945/amoba:latest
```

# RUN the proof
```
docker run --mount type=bind,source="$(pwd)",target=/amoba -it doma945/amoba:latest bash
cd amoba
./docker/install.sh
cd /amoba/build
./AMOBA
```

# Test the proof

```
// from the above container
./AMOBA --test
```