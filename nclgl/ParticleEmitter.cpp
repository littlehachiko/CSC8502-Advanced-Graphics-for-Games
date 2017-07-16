#include "ParticleEmitter.h"
#include <string>
#include <io.h>
#include <stdio.h>
#include <random>

std::default_random_engine generatorExplo;
std::default_random_engine generatorSnow;
std::default_random_engine generatorVolcano;

//Cheap random number generator, will generate
//numbers between 0.0 and 1.0 to 2 DP
//#define RAND() ((rand()%101)/100.0f)

/*
Constructor, which sets everything to some 'sensible' defaults.
*/
ParticleEmitter::ParticleEmitter(char tex[16])	{
	particleRate		= 100.0f;
	particleLifetime	= 500.0f;
	particleSize		= 24.0f;
	particleVariance	= 0.2f;
	nextParticleTime	= 0.0f;
	particleSpeed		= 0.2f;
	numLaunchParticles	= 10;
	largestSize			= 0;

	/*
	Each particle is a white dot, which has an alpha fade on it,
	so the edges fade to 0.0 alpha.
	*/
	//texture = SOIL_load_OGL_texture("../Textures/particle.tga",
	//SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT);

	//texture = SOIL_load_OGL_texture(TEXTUREDIR"fire.jpg",
	//	SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
	
	texture = SOIL_load_OGL_texture(tex,
		SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT);
}

/*
In our destructor, we must make sure we delete the particles held in BOTH
of our lists!
*/
ParticleEmitter::~ParticleEmitter(void)	{
	for(std::vector<Particle *>::iterator i = particles.begin(); i != particles.end(); ++i) {
		delete (*i);
	}
	for(std::vector<Particle *>::iterator i = freeList.begin(); i != freeList.end(); ++i) {
		delete (*i);
	}
}

/*
Every frame, we're going to call this Update function. In a 'real' game, this maybe wouldn't
be the Renderer's job, but maybe part of the gameplay 'entity' system. Depends on what
the game components can 'see' (i.e are we hiding what type of API we're using for rendering
from the rest of the game, etc).

It goes through every particle of this emitter, and updates it. if it has no 'life' left,
it is removed from the particle list. If it's time to generate some new particles, we do
that in here, too. Finally, this function resizes our VBOs if necessary. 
*/
void ParticleEmitter::UpdateExplo(float msec)	{
	nextParticleTime -= msec;	//some time has passed!

	/*
	Enough time might have passed that more than 1 'launch' of particles is necessary...
	*/
	while(nextParticleTime <= 0) {
		nextParticleTime += particleRate;
		//Add a number of particles to the vector, obtained from the free list.
		for(int i = 0; i < numLaunchParticles; ++i) {
			particles.push_back(GetFreeParticleExplo());
		}
	}

	//Now for the particle 'think' function. Particles are so 'lightweight' there's not
	//much point putting this as a member variable of the Particle struct...


	for(std::vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */) {
		Particle *p = (*i);

		//We're keeping the particles 'life' in the alpha channel of its colour. 
		//This means it'll also fade out as it loses energy. Kinda cool?
		p->colour.w -= (msec / particleLifetime);

		//If this particle has ran out of life, remove it from the 'active' list,
		//and put it on the 'free' list for reuse later.
		if(p->colour.w <= 0.0f) {
			freeList.push_back(p);
			i = particles.erase(i);
		}
		else{
			//Otherwise, this particle must be still 'alive'. Update its
			//position by multiplying its normalised direction by the
			//particle speed, and adding the result to the position. Easy!

			p->position += p->direction*(msec*particleSpeed);
			++i;	
			//Only update our iterator value here.
			//If we didn't do this, we'd skip over particles when removing
			//particles from the list...
		}
	}

	//If we now have more particles than we have graphics memory for, we
	//must allocate some new space for them, using ResizeArrays.
	if(particles.size() > largestSize) {
		ResizeArrays();	
	}
}

void ParticleEmitter::UpdateSnow(float msec) {

	nextParticleTime -= msec;	//some time has passed!

	while (nextParticleTime <= 0)
	{
		nextParticleTime += particleRate;

		for (int i = 0; i < numLaunchParticles; ++i)
		{
			particles.push_back(GetFreeParticleSnow());
		}
	}

	for (std::vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */)
	{
		Particle *p = (*i);

		p->colour.w -= (msec / particleLifetime);

		if (p->colour.w <= 0.0f)
		{
			freeList.push_back(p);
			i = particles.erase(i);
		}
		else
		{
			p->position += p->direction*(msec*particleSpeed);
			p->position.y -= (msec * msec * msec * (p->gravity)) / 2000.0f;
			p->gravity += msec * 0.01f;

			++i;
		}
	}

	if (particles.size() > largestSize)
	{
		ResizeArrays();
	}
}

void ParticleEmitter::UpdateVolcano(float msec) {
	std::uniform_real_distribution<float> dis1(0.f, 1.f);

	nextParticleTime -= msec;	//some time has passed!

	while (nextParticleTime <= 0)
	{
		nextParticleTime += particleRate;

		for (int i = 0; i < numLaunchParticles; ++i)
		{
			particles.push_back(GetFreeParticleVolcano());
		}
	}

	for (std::vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */)
	{
		Particle *p = (*i);

		p->colour.w -= (msec / particleLifetime);

		if (p->colour.w <= 0.0f)
		{
			freeList.push_back(p);
			i = particles.erase(i);
		}
		else
		{
			p->position += p->direction*(msec*particleSpeed);
		//	p->position -= (msec * msec * msec * (p->gravity)) / 1000.0f;
			p->direction.y -= msec * (p->gravity) * 0.001f;
			//p->direction.y -= msec * (p->gravity) * 0.01f * dis1(generatorVolcano);
			++i;
		}
	}

	if (particles.size() > largestSize)
	{
		ResizeArrays();
	}
}
/*
This function gets a pointer to an unused particle - either from the freelist,
or from a newly created particle on the heap.
*/
Particle* ParticleEmitter::GetFreeParticleExplo()	{
	Particle * p = NULL;

	//If we have a spare particle on the freelist, pop it off!
	if(freeList.size() > 0) {
		p = freeList.back();
		freeList.pop_back();
	}
	else{ 
		//no spare particles, so we need a new one
		p = new Particle();
	}

	//Now we have to reset its values - if it was popped off the
	//free list, it'll still have the values of its 'previous life'
	std::uniform_real_distribution<float> dis1(0.f, 1.f);
	std::uniform_real_distribution<float> dis2(-1.f, 1.f);

	//p->colour		= Vector4(1,1,1,1.0);
	p->colour = Vector4(1.0f, 0.4f, 0.0f, 1.0f);
	p->direction	= initialDirection;
	p->direction.x += (dis2(generatorExplo)* particleVariance);
	p->direction.y += (dis2(generatorExplo)* particleVariance);
	p->direction.z += (dis2(generatorExplo)* particleVariance);

	p->direction.Normalise();	//Keep its direction normalised!
	p->position.ToZero();

	return p;	//return the new particle :-)
}

Particle* ParticleEmitter::GetFreeParticleSnow(){

	Particle * p = NULL;

	//If we have a spare particle on the freelist, pop it off!
	if (freeList.size() > 0)
	{
		p = freeList.back();
		freeList.pop_back();
	}
	else
	{
		//no spare particles, so we need a new one
		p = new Particle();
	}

	//Now we have to reset its values - if it was popped off the
	//free list, it'll still have the values of its 'previous life'
	std::uniform_real_distribution<float> dis1(0.f, 1.f);
	std::uniform_real_distribution<float> dis2(-1.f, 1.f);


	p->gravity = 0.6f;
	p->colour = Vector4(1,1,1,1.0);

	//p->colour = Vector4(
	//	dis1(generatorVolcano),
	//	dis1(generatorVolcano),
	//	dis1(generatorVolcano),
	//	1.0);

	p->direction = initialDirection;
	p->direction.x += (dis2(generatorSnow) * particleVariance);
	p->direction.y = 1.0f;
	p->direction.z += (dis2(generatorSnow) * particleVariance);

	p->direction.Normalise();	//Keep its direction normalised!
	//p->position.ToZero();
	p->position = Vector3 (
		dis1(generatorSnow) * 10000.0f, 
		0.0f, 
		dis1(generatorVolcano) * 10000.0f
	);

	return p;	//return the new particle :-)
}

Particle* ParticleEmitter::GetFreeParticleVolcano() {

	Particle * p = NULL;

	//If we have a spare particle on the freelist, pop it off!
	if (freeList.size() > 0)
	{
		p = freeList.back();
		freeList.pop_back();
	}
	else
	{
		//no spare particles, so we need a new one
		p = new Particle();
	}

	//Now we have to reset its values - if it was popped off the
	//free list, it'll still have the values of its 'previous life'
	std::uniform_real_distribution<float> dis1(0.f, 1.f);
	std::uniform_real_distribution<float> dis2(-1.f, 1.f);


	p->gravity = 2.0f;
	p->colour = Vector4(1.0f, 0.4f, 0.0f, 1.0f);

	p->direction = initialDirection;
	p->direction.x += (dis2(generatorVolcano) * particleVariance);
	p->direction.y = 1.0f;
	p->direction.z += (dis2(generatorVolcano) * particleVariance);

	

	p->direction.Normalise();	//Keep its direction normalised!
	p->position.ToZero();
	//p->position = Vector3(512 * 16.0f / 2.0f, 2000, 512 * 16.0f);

	return p;	//return the new particle :-)
}

/*
If we end up with more particles than space in graphics memory, we must
allocate some new graphics memory!
*/
void	ParticleEmitter::ResizeArrays() {
	//Delete the system memory, as it is of the wrong size now...
	//yes, this does mean we're keeping particle info in THREE places...
	delete[] vertices;
	delete[] colours;

	//Tell OpenGL that we can delete our old VBOs
	glDeleteBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glDeleteBuffers(1, &bufferObject[COLOUR_BUFFER]);

	//Make some new system memory
	vertices = new Vector3[particles.size()];
	colours  = new Vector4[particles.size()];

	//Make some new VBO space in memory. Note how we don't actually upload any
	//data to the graphics card here! We just 'new' the memory for now.
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, particles.size()*sizeof(Vector3), 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, particles.size()*sizeof(Vector4), 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	//Done with our buffers...

	largestSize = particles.size();		//Update our largest particles size
}

/*
Now for the overloaded Draw function. It's a little bit messy, as we have to copy all of the
Particle data structs into one contiguous lump of memory to send using the OpenGL command
glBufferSubData - which you should remember from the skeletal animation tutorial. 

Why don't we use glMapBuffer? Just uploading a whole load of new data is probably easier
and quicker than faffing with mapping and unmapping buffers!
*/
void ParticleEmitter::Draw()	{
	//Get 2 contiguous sections of memory full of our particle info
	for(unsigned int i = 0; i < particles.size(); ++i) {
		vertices[i] = particles.at(i)->position;
		colours[i]  = particles.at(i)->colour;
	}

	glBindVertexArray(arrayObject);

	//Bind our vertex data, and update its data with that of the vertices array
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size()*sizeof(Vector3), (void*)vertices);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);	//Tell the VAO we have positions...
	glEnableVertexAttribArray(VERTEX_BUFFER);	//Binds this buffer to the VAO
	
	//And now do the same for colours...
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size()*sizeof(Vector4), (void*)colours);
	glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
	glEnableVertexAttribArray(COLOUR_BUFFER);
	
	//We're going to use a type of alpha blending known as additive blending here. Overlapping 
	//particles will have their colours added together on screen, giving us a cool effect when
	//particles are near each other. Fire a lot of particles at once with slow speed to get a
	//'plasma ball' type effect!
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	//And now do our usual Drawing stuff...
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_POINTS,  0, particles.size());	// draw ordered list of vertices
	glBindTexture(GL_TEXTURE_2D, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0); //Remember to turn off our VAO ;)
};