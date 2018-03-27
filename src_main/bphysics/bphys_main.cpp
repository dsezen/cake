/******************************************************************************
 * Copyright (C) 2018 Deniz Sezen
 ******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ******************************************************************************
 */

///////////////////////////////////////////////////////////////////////////////
// Main physics module function.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

/** Standard library includes. */
#include <cstdarg>

/** Bullet physics includes. */
#include "btBulletDynamicsCommon.h"

extern "C"
{
    /** Shared code includes. */
    #include "q_shared.h"
    #include "bphysics_shared.h"
}

///////////////////////////////////////////////////////////////////////////////
// Globals
///////////////////////////////////////////////////////////////////////////////

/** List of exported functions and data from this module. */
physics_export_t globals;

/** List of imported functions from the engine. */
physics_import_t pi;

/** Bullet physics globals. */
btDefaultCollisionConfiguration* collisionConfig = nullptr;
btCollisionDispatcher* dispatcher = nullptr;
btBroadphaseInterface* overlappingPairCache = nullptr;
btSequentialImpulseConstraintSolver* solver = nullptr;
btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
btAlignedObjectArray<btCollisionShape*> collisionShapes;

///////////////////////////////////////////////////////////////////////////////
// Forward declarations.
///////////////////////////////////////////////////////////////////////////////

/** Subsystem management. */
extern "C"
{
    void InitPhysics(void);
    void ShutdownPhysics(void);
    void UpdatePhysics(double dt_msec);
}

///////////////////////////////////////////////////////////////////////////////
// Exported
///////////////////////////////////////////////////////////////////////////////

/**
 * GetPhysicsAPI
 *
 * Returns a pointer to the structure with all entry points and global
 * variables for this module.
 */
extern "C" Q_DLL_EXPORT physics_export_t* GetPhysicsAPI(physics_import_t* import)
{
    pi = *import;

    globals.apiversion  = PHYSICS_API_VERSION;
    globals.Init        = InitPhysics;
    globals.Shutdown    = ShutdownPhysics;
    globals.Update      = UpdatePhysics;

    return &globals;
}

///////////////////////////////////////////////////////////////////////////////
// Module functions
///////////////////////////////////////////////////////////////////////////////

/**
 * InitPhysics
 * 
 * Initializes the physics system.
 */
extern "C" void InitPhysics(void)
{
    // Allocate the default collision configuration.
    collisionConfig = new btDefaultCollisionConfiguration();

    // Allocate collision dispatcher
    dispatcher = new btCollisionDispatcher(collisionConfig);

    // Allocate broadphase interface
    overlappingPairCache = new btDbvtBroadphase();

    // Setup default constraint solver.
    solver = new btSequentialImpulseConstraintSolver;

    // Setup dynamics world
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, 
        overlappingPairCache, solver, collisionConfig);
}

/**
 * ShutdownPhysics
 *
 * Cleans up and shuts down the physics system.
 */
extern "C" void ShutdownPhysics(void)
{

    // Remove rigid bodies from the dynamics world
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(body);
        delete obj;
    }

    //delete collision shapes
    for (int j = 0; j < collisionShapes.size(); j++)
    {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }

    //delete dynamics world
    delete dynamicsWorld;
    dynamicsWorld = nullptr;

    //delete solver
    delete solver;
    solver = nullptr;

    //delete broadphase
    delete overlappingPairCache;
    overlappingPairCache = nullptr;

    //delete dispatcher
    delete dispatcher;
    dispatcher = nullptr;

    //delete collision config
    delete collisionConfig;
    collisionConfig = nullptr;

    //clear up shapes list
    collisionShapes.clear();
}

/**
 * UpdatePhysics
 *
 * Called every frame. Runs one (or more) steps of the simulation.
 */
extern "C" void UpdatePhysics(double dt_msec)
{
    if (!dynamicsWorld)
        return;

    // Convert msec to sec
    const double dt_sec = dt_msec * 0.001;

    // -----------------------------------------------------
    // Note: We're assuming a max of 95 fps and a min of 15.
    // -----------------------------------------------------
    // The preferred equation that must be satisfied is:
    //  timeStep < maxSubSteps * fixedTimeStep.
    //
    // Where:
    //  timeStep = dt_sec
    // -----------------------------------------------------
    dynamicsWorld->stepSimulation(dt_sec);
}

///////////////////////////////////////////////////////////////////////////////
// Shared functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Com_Printf
 *
 * Prints data to the given string.
 */
extern "C" void Com_Printf(char *msg, ...)
{
    va_list		argptr;
    char		text[1024];

    va_start(argptr, msg);
    vsprintf(text, msg, argptr);
    va_end(argptr);

    pi.dprintf("%s", text);
}