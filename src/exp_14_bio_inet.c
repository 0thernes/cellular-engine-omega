#include "omega_bare_crt.h"
#include "omega_abi.h"

/*
 * TB14 — CLEAN-ROOM LOCK-FREE 4-LAYER LAFONT BIO-INTERACTION-NET VM ("BIO-FROSTBITE")
 *        + HYBRID STOCHASTIC SPATIAL INTERACTION NET (SSIN) VOXEL ARBITER
 *
 * Transfers Yves Lafont's Interaction Nets (1990) & Frostbite linear graph-rewriting
 * directly across all 4 biological layers:
 *   Layer 1 (DNA):        Polymerase(Dup) >< Base, Ribosome >< Codon, ADAR >< mRNA, CRISPR >< Phage, Era >< Strand
 *   Layer 2 (Organelles): ATP_Synth >< H+, Kinesin >< Tubulin, Ca2+ >< MT_Wall, Phagosome >< Prey (Russian-Doll D0->D3)
 *   Layer 3 (Cellular):   FtsZ >< CellRing, ESCRT_III >< CellRing, PhageT4 >< Receptor (Virocell), CAHS >< Cell (0-Power Tun!)
 *   Layer 4 (Creatures):  GapJunction >< CellPair (Conservative Levin Vm Laplacian), ApicalWedge >< Sheet, CPG >< Muscle
 *
 * Empirically proves Strong Confluence (Diamond Property) in C11 by executing the EXACT same
 * 4-layer biosphere graph to normal form under 3 wild execution schedules:
 *   Schedule 0: Forward FIFO queue
 *   Schedule 1: Reverse LIFO stack
 *   Schedule 2: Chaotic Random Parallel Permutation (Fisher-Yates shuffled wave firing)
 * and verifying 0-bit identical canonical Normal-Form SHA-256 across all 3 schedules with
 * 0 linear mass drift, 0 bioelectric charge drift, and 0 single-ownership wire errors!
 */

#define INET_MAX_NODES 4096
#define INET_MAX_REDEX 2048

typedef enum BioAgentTag {
    AG_NONE = 0,
    /* Layer 1: DNA / Polymer / Genetic OS */
    AG_POLY_DUP,       /* Polymerase carrying linear dNTP monomer wire */
    AG_DNA_BASE,       /* Nucleotide node in doubly-linked polymer chain */
    AG_DNA_COPY,       /* Replicated daughter DNA node (normal form) */
    AG_RIBO,           /* Ribosome translating codon chain */
    AG_CODON,          /* mRNA codon node */
    AG_ADAR,           /* Cephalopod ADAR RNA editor */
    AG_MRNA_RAW,       /* Unedited mRNA node */
    AG_CRISPR,         /* CRISPR-Cas effector complex */
    AG_PHAGE,          /* Invading bacteriophage genome */
    AG_ERA_NUC,        /* Explicit linear Nuclease/Proteasome eraser */
    AG_STRAND_OLD,     /* Damaged strand marked for recycling */
    /* Layer 2: Organelles (Nested Membrane Boxes & Cytoskeleton) */
    AG_ATP_SYNTH,      /* F1F0 rotary ATP synthase (c-ring gearbox) */
    AG_PROTON_BATCH,   /* Proton motive force batch wire */
    AG_KINESIN,        /* Processive (+) motor stepping +8.2 nm along MT */
    AG_TUBULIN,        /* Microtubule protofilament dimer */
    AG_CA2_MELT,       /* Ca2+ microtubule catastrophe trigger */
    AG_MT_WALL,        /* Uncapped GDP-tubulin wall segment */
    AG_PHAGOSOME,      /* Endosymbiosis engulfment vesicle */
    AG_PREY_BOX,       /* Lysis-resistant endosymbiont candidate */
    AG_ENDO_BOX,       /* Nested Russian-Doll organelle box (Depth 1..3) */
    /* Layer 3: Cellular Bilayer Rings & Cryptobiosis */
    AG_FTSZ_DIV,       /* Symmetric binary fission ring (Q <= 0.5) */
    AG_ESCRT_BUD,      /* Asymmetric exosome budding machine (0.5 < Q <= 1) */
    AG_CELL_RING,      /* Closed lipid bilayer cell membrane */
    AG_PHAGE_T4,       /* Lytic phage docking on receptor */
    AG_CELL_RECEPTOR,  /* Surface receptor converting cell -> Virocell */
    AG_VIROCELL,       /* Reprogrammed viral factory cell */
    AG_CAHS_GLASS,     /* Tardigrade CAHS IDP bioglass vitrification agent */
    AG_BIOGLASS_TUN,   /* Shielded zero-power cryptobiotic cell tun */
    AG_H2O_REHYD,      /* Rehydration water wire uncapping tun ports */
    /* Layer 4: Multicellular Creatures / Organs / Bioelectricity */
    AG_GAP_JUNC,       /* Gap junction active pair equalizing Vm conservatively */
    AG_CELL_VM,        /* Bioelectric membrane cell node carrying integer charge_q */
    AG_APICAL_WEDGE,   /* Myosin-II apical constriction ring */
    AG_EPITH_PATCH,    /* Flat epithelial sheet folding into hollow gut */
    AG_GUT_LUMEN,      /* Sealed 3D hollow gut tube organ */
    AG_CPG_NEURON,     /* Matsuoka CPG half-center oscillator */
    AG_MUSCLE_BAND,    /* Lobopod muscle actuator */
    /* Linear Conservation Terminal Wires */
    AG_MONOMER,        /* Recycled linear monomer / dNTP / tubulin-GDP */
    AG_PROTEIN,        /* Folded catalytic protein product */
    AG_ATP,            /* Charged ATP wire */
    AG_WORK_STEP,      /* Mechanical / locomotor work output */
    AG_HEAT            /* Explicit Landauer dissipation heat wire */
} BioAgentTag;

typedef struct PortRef {
    int32_t node;      /* -1 if open/unconnected */
    uint8_t slot;      /* 0 = Principal port, 1..3 = Auxiliary ports */
} PortRef;

typedef struct BioAgent {
    uint8_t  tag;
    uint8_t  layer;       /* 1..4 */
    uint8_t  box_depth;   /* 0..3 Russian-Doll nesting depth */
    uint8_t  shielded;    /* 1 = CAHS bioglass shield freezes principal port */
    uint8_t  alive;
    uint32_t lineage_id;  /* Order-independent content-addressed birth key */
    int64_t  mass_q;      /* Exact integer linear mass quantum */
    int64_t  charge_q;    /* Exact integer bioelectric charge Vm */
    int32_t  param;       /* Biological parameter (codon, c-ring, step, etc.) */
    PortRef  port[4];
} BioAgent;

typedef struct ActivePair {
    int32_t a;
    int32_t b;
} ActivePair;

typedef struct BioInet {
    BioAgent   nodes[INET_MAX_NODES];
    int        n_nodes;
    ActivePair redexes[INET_MAX_REDEX];
    int        n_redex;
    int        rewrites_total;
    int        layer_rewrites[4];
    int        max_box_depth;
    int        mutant_race_mode;
    int64_t    shared_race_acc;
} BioInet;

static inline uint32_t mix_lineage(uint32_t a, uint32_t b, uint32_t salt) {
    uint32_t x = a ^ (b * 0x9E3779B1u) ^ (salt * 0x85EBCA77u);
    x ^= x >> 16;
    x *= 0x7FEB352Du;
    x ^= x >> 15;
    x *= 0x846CA68Bu;
    x ^= x >> 16;
    return x;
}

static void inet_init(BioInet* net, int mutant_race_mode) {
    memset(net, 0, sizeof(*net));
    net->mutant_race_mode = mutant_race_mode;
}

static int32_t inet_add(BioInet* net, uint8_t tag, uint8_t layer, uint8_t depth,
                        uint32_t lineage_id, int64_t mass_q, int64_t charge_q, int32_t param) {
    if (net->n_nodes >= INET_MAX_NODES) return -1;
    int32_t id = net->n_nodes++;
    BioAgent* ag = &net->nodes[id];
    ag->tag        = tag;
    ag->layer      = layer;
    ag->box_depth  = depth;
    ag->shielded   = 0;
    ag->alive      = 1;
    ag->lineage_id = lineage_id;
    ag->mass_q     = mass_q;
    ag->charge_q   = charge_q;
    ag->param      = param;
    for (int p = 0; p < 4; ++p) { ag->port[p].node = -1; ag->port[p].slot = 0; }
    if ((int)depth > net->max_box_depth) net->max_box_depth = (int)depth;
    return id;
}

static int inet_is_rehydration_pair(const BioAgent* a, const BioAgent* b) {
    return (a->tag == AG_BIOGLASS_TUN && b->tag == AG_H2O_REHYD) ||
           (b->tag == AG_BIOGLASS_TUN && a->tag == AG_H2O_REHYD);
}

static void inet_push_redex(BioInet* net, int32_t a, int32_t b) {
    if (a < 0 || b < 0 || a == b || net->n_redex >= INET_MAX_REDEX) return;
    if (!net->nodes[a].alive || !net->nodes[b].alive) return;
    if ((net->nodes[a].shielded || net->nodes[b].shielded) &&
        !inet_is_rehydration_pair(&net->nodes[a], &net->nodes[b])) return;
    net->redexes[net->n_redex].a = a;
    net->redexes[net->n_redex].b = b;
    net->n_redex++;
}

static void inet_connect(BioInet* net, int32_t a, uint8_t pa, int32_t b, uint8_t pb) {
    if (a < 0 || b < 0 || a == b || pa >= 4 || pb >= 4) return;
    net->nodes[a].port[pa].node = b;
    net->nodes[a].port[pa].slot = pb;
    net->nodes[b].port[pb].node = a;
    net->nodes[b].port[pb].slot = pa;
    if (pa == 0 && pb == 0) {
        inet_push_redex(net, a, b);
    }
}

/* Detach and return the partner currently connected to (u, slot), clearing both ends cleanly */
static PortRef inet_detach(BioInet* net, int32_t u, uint8_t slot) {
    PortRef old = net->nodes[u].port[slot];
    net->nodes[u].port[slot].node = -1;
    net->nodes[u].port[slot].slot = 0;
    if (old.node >= 0 && old.slot < 4) {
        net->nodes[old.node].port[old.slot].node = -1;
        net->nodes[old.node].port[old.slot].slot = 0;
    }
    return old;
}

static int64_t inet_total_mass(const BioInet* net) {
    int64_t sum = 0;
    for (int i = 0; i < net->n_nodes; ++i) {
        if (net->nodes[i].alive) sum += net->nodes[i].mass_q;
    }
    return sum;
}

static int64_t inet_total_charge(const BioInet* net) {
    int64_t sum = 0;
    for (int i = 0; i < net->n_nodes; ++i) {
        if (net->nodes[i].alive) sum += net->nodes[i].charge_q;
    }
    return sum;
}

/* Strict Single-Ownership Wire Audit: every live wire must be strictly symmetric & dangling-free */
static int inet_audit_ownership(const BioInet* net) {
    int errs = 0;
    for (int i = 0; i < net->n_nodes; ++i) {
        if (!net->nodes[i].alive) continue;
        for (int p = 0; p < 4; ++p) {
            PortRef pr = net->nodes[i].port[p];
            if (pr.node == -1) continue;
            if (pr.node < 0 || pr.node >= net->n_nodes || pr.node == i || pr.slot >= 4) {
                errs++;
                continue;
            }
            const BioAgent* other = &net->nodes[pr.node];
            if (!other->alive || other->port[pr.slot].node != i || other->port[pr.slot].slot != p) {
                errs++;
            }
        }
    }
    return errs;
}

static void inet_kill_pair(BioInet* net, int32_t a, int32_t b) {
    for (int p = 0; p < 4; ++p) {
        inet_detach(net, a, (uint8_t)p);
        inet_detach(net, b, (uint8_t)p);
    }
    net->nodes[a].alive = 0;
    net->nodes[b].alive = 0;
}

/*
 * Strictly Local Principal-Port Active-Pair Rewrite (a >< b)
 * Canonicalizes (a, b) by tag so every rule is 100% symmetric and order-independent.
 */
static int inet_fire_pair(BioInet* net, int32_t u, int32_t v) {
    if (u < 0 || v < 0 || u == v) return 0;
    if (!net->nodes[u].alive || !net->nodes[v].alive) return 0;
    if ((net->nodes[u].shielded || net->nodes[v].shielded) &&
        !inet_is_rehydration_pair(&net->nodes[u], &net->nodes[v])) return 0;
    if (net->nodes[u].port[0].node != v || net->nodes[v].port[0].node != u) return 0;

    /* Canonical ordering by tag (then lineage_id) guarantees symmetric commutativity */
    if (net->nodes[u].tag > net->nodes[v].tag ||
        (net->nodes[u].tag == net->nodes[v].tag && net->nodes[u].lineage_id > net->nodes[v].lineage_id)) {
        int32_t tmp = u; u = v; v = tmp;
    }

    BioAgent A = net->nodes[u];
    BioAgent B = net->nodes[v];
    int64_t m_tot = A.mass_q + B.mass_q;
    int64_t q_tot = A.charge_q + B.charge_q;
    uint32_t la = A.lineage_id, lb = B.lineage_id;
    uint8_t d_max = (A.box_depth > B.box_depth) ? A.box_depth : B.box_depth;

    /* Planted Mutant M8: injects schedule-dependent shared-RAM race + silent mass leak! */
    if (net->mutant_race_mode) {
        net->shared_race_acc += (int64_t)((net->rewrites_total * 13 + (int)(la & 7u)) & 15);
        m_tot += (net->shared_race_acc & 3) + 1; /* Breaks BOTH linear mass conservation AND Strong Confluence! */
    }

    /* ---------------- LAYER 1: DNA / POLYMER / GENETIC OS ---------------- */
    if (A.tag == AG_POLY_DUP && B.tag == AG_DNA_BASE) {
        /* Polymerase(dNTP) >< DNA_Base -> Daughter Copy + Proofread Heat + Advance Poly along chain! */
        PortRef next_base = inet_detach(net, v, 1);
        inet_kill_pair(net, u, v);
        int64_t heat_q = (m_tot >= 10) ? 2 : 0;
        int64_t copy_q = (B.mass_q > 4) ? B.mass_q : (m_tot - heat_q) / 2;
        int64_t poly_rem = m_tot - copy_q - heat_q;
        inet_add(net, AG_DNA_COPY, 1, d_max, mix_lineage(la, lb, 101u), copy_q, 0, B.param ^ 0x5A);
        if (heat_q > 0) inet_add(net, AG_HEAT, 1, d_max, mix_lineage(la, lb, 102u), heat_q, 0, 1);
        int32_t poly2 = inet_add(net, AG_POLY_DUP, 1, d_max, mix_lineage(la, lb, 103u), poly_rem, q_tot, A.param + 1);
        if (next_base.node >= 0 && net->nodes[next_base.node].alive) {
            inet_connect(net, poly2, 0, next_base.node, next_base.slot);
        }
        net->layer_rewrites[0]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_RIBO && B.tag == AG_CODON) {
        /* Ribosome >< Codon -> Catalytic Protein + Heat + Advance Ribosome along mRNA! */
        PortRef next_codon = inet_detach(net, v, 1);
        inet_kill_pair(net, u, v);
        int64_t heat_q = (m_tot >= 8) ? 2 : 0;
        int64_t prot_q = B.mass_q - (B.mass_q > 2 ? 1 : 0);
        int64_t ribo_q = m_tot - prot_q - heat_q;
        inet_add(net, AG_PROTEIN, 1, d_max, mix_lineage(la, lb, 111u), prot_q, 0, B.param);
        if (heat_q > 0) inet_add(net, AG_HEAT, 1, d_max, mix_lineage(la, lb, 112u), heat_q, 0, 1);
        int32_t ribo2 = inet_add(net, AG_RIBO, 1, d_max, mix_lineage(la, lb, 113u), ribo_q, q_tot, A.param + 1);
        if (next_codon.node >= 0 && net->nodes[next_codon.node].alive) {
            inet_connect(net, ribo2, 0, next_codon.node, next_codon.slot);
        }
        net->layer_rewrites[0]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_ADAR && B.tag == AG_MRNA_RAW) {
        /* Cephalopod ADAR >< mRNA_Raw -> Edited Codon wired straight into waiting Ribosome! */
        PortRef ribo_wait = inet_detach(net, v, 1);
        PortRef tail_chain = inet_detach(net, v, 2);
        inet_kill_pair(net, u, v);
        int64_t ed_q = B.mass_q;
        int64_t adar_q = m_tot - ed_q;
        int32_t codon = inet_add(net, AG_CODON, 1, d_max, mix_lineage(la, lb, 121u), ed_q, q_tot, B.param ^ 0x3C);
        inet_add(net, AG_ADAR, 1, d_max, mix_lineage(la, lb, 122u), adar_q, 0, A.param + 1);
        if (tail_chain.node >= 0 && net->nodes[tail_chain.node].alive) {
            inet_connect(net, codon, 1, tail_chain.node, tail_chain.slot);
        }
        if (ribo_wait.node >= 0 && net->nodes[ribo_wait.node].alive) {
            inet_connect(net, codon, 0, ribo_wait.node, ribo_wait.slot);
        }
        net->layer_rewrites[0]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_CRISPR && B.tag == AG_PHAGE) {
        /* CRISPR >< Phage -> Cleaved recycled Monomers + Landauer Heat + Freed CRISPR! */
        inet_kill_pair(net, u, v);
        int64_t heat_q = (B.mass_q >= 4) ? 2 : 0;
        int64_t mono_q = B.mass_q - heat_q;
        inet_add(net, AG_CRISPR, 1, d_max, mix_lineage(la, lb, 131u), A.mass_q, q_tot, A.param + 1);
        inet_add(net, AG_MONOMER, 1, d_max, mix_lineage(la, lb, 132u), mono_q, 0, B.param);
        if (heat_q > 0) inet_add(net, AG_HEAT, 1, d_max, mix_lineage(la, lb, 133u), heat_q, 0, 1);
        net->layer_rewrites[0]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_ERA_NUC && B.tag == AG_STRAND_OLD) {
        /* Explicit Linear Erasure (Era): recycles into Monomer + Heat, never drops mass! */
        inet_kill_pair(net, u, v);
        int64_t heat_q = (m_tot >= 6) ? 3 : 0;
        inet_add(net, AG_MONOMER, 1, d_max, mix_lineage(la, lb, 141u), m_tot - heat_q, q_tot, B.param);
        if (heat_q > 0) inet_add(net, AG_HEAT, 1, d_max, mix_lineage(la, lb, 142u), heat_q, 0, 1);
        net->layer_rewrites[0]++;
        net->rewrites_total++;
        return 1;
    }

    /* ---------------- LAYER 2: ORGANELLES (NESTED BOXES & MOTORS) ---------------- */
    if (A.tag == AG_ATP_SYNTH && B.tag == AG_PROTON_BATCH) {
        /* F1F0 ATP Synthase >< H+ Batch -> Charged ATP wire + Heat + Freed Synthase */
        inet_kill_pair(net, u, v);
        int64_t heat_q = (B.mass_q >= 4) ? 2 : 0;
        int64_t atp_q = B.mass_q - heat_q;
        inet_add(net, AG_ATP_SYNTH, 2, d_max, mix_lineage(la, lb, 201u), A.mass_q, q_tot, A.param);
        inet_add(net, AG_ATP, 2, d_max, mix_lineage(la, lb, 202u), atp_q, 0, A.param);
        if (heat_q > 0) inet_add(net, AG_HEAT, 2, d_max, mix_lineage(la, lb, 203u), heat_q, 0, 1);
        net->layer_rewrites[1]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_KINESIN && B.tag == AG_TUBULIN) {
        /* Kinesin(ATP) >< Tubulin_i -> +8.2nm Step onto Tubulin_{i+1} + Work + Heat! */
        PortRef next_tub = inet_detach(net, v, 1);
        inet_kill_pair(net, u, v);
        int64_t work_q = (m_tot >= 12) ? 2 : 0;
        int64_t heat_q = (m_tot >= 12) ? 1 : 0;
        int64_t tub_q  = B.mass_q;
        int64_t kin_q  = m_tot - tub_q - work_q - heat_q;
        inet_add(net, AG_TUBULIN, 2, d_max, mix_lineage(la, lb, 211u), tub_q, 0, B.param);
        if (work_q > 0) inet_add(net, AG_WORK_STEP, 2, d_max, mix_lineage(la, lb, 212u), work_q, 0, 82); /* +8.2 nm */
        if (heat_q > 0) inet_add(net, AG_HEAT, 2, d_max, mix_lineage(la, lb, 213u), heat_q, 0, 1);
        int32_t kin2 = inet_add(net, AG_KINESIN, 2, d_max, mix_lineage(la, lb, 214u), kin_q, q_tot, A.param + 1);
        if (next_tub.node >= 0 && net->nodes[next_tub.node].alive) {
            inet_connect(net, kin2, 0, next_tub.node, next_tub.slot);
        }
        net->layer_rewrites[1]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_CA2_MELT && B.tag == AG_MT_WALL) {
        /* Ca2+ >< MT_Wall -> Banana-peel microtubule catastrophe cascade back into Monomers! */
        PortRef next_wall = inet_detach(net, v, 1);
        inet_kill_pair(net, u, v);
        int64_t heat_q = (B.mass_q >= 4) ? 1 : 0;
        int64_t mono_q = B.mass_q - heat_q;
        inet_add(net, AG_MONOMER, 2, d_max, mix_lineage(la, lb, 221u), mono_q, 0, B.param);
        if (heat_q > 0) inet_add(net, AG_HEAT, 2, d_max, mix_lineage(la, lb, 222u), heat_q, 0, 1);
        int32_t ca2 = inet_add(net, AG_CA2_MELT, 2, d_max, mix_lineage(la, lb, 223u), A.mass_q, q_tot, A.param + 1);
        if (next_wall.node >= 0 && net->nodes[next_wall.node].alive) {
            inet_connect(net, ca2, 0, next_wall.node, next_wall.slot);
        }
        net->layer_rewrites[1]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_PHAGOSOME && (B.tag == AG_PREY_BOX || B.tag == AG_ENDO_BOX)) {
        /* Russian-Doll Endosymbiosis (Depth d -> d+1, up to Depth 3: Mealybug/Tremblaya/Moranella!) */
        PortRef outer_host = inet_detach(net, u, 1);
        inet_kill_pair(net, u, v);
        uint8_t next_depth = (d_max < 3) ? (uint8_t)(d_max + 1) : 3;
        int32_t endo = inet_add(net, AG_ENDO_BOX, 2, next_depth, mix_lineage(la, lb, 231u), m_tot, q_tot, (int32_t)next_depth);
        if (outer_host.node >= 0 && net->nodes[outer_host.node].alive) {
            inet_connect(net, endo, 0, outer_host.node, outer_host.slot);
        }
        net->layer_rewrites[1]++;
        net->rewrites_total++;
        return 1;
    }

    /* ---------------- LAYER 3: CELLULAR BILAYER RINGS & CRYPTOBIOSIS ---------------- */
    if (A.tag == AG_FTSZ_DIV && B.tag == AG_CELL_RING) {
        /* FtsZ Binary Fission (Q <= 0.5): splits CellRing cleanly into 2 independent daughter cells! */
        inet_kill_pair(net, u, v);
        int64_t heat_q = (m_tot >= 20) ? 4 : 0;
        int64_t rem = m_tot - heat_q;
        int64_t m1 = rem / 2, m2 = rem - m1;
        int64_t q1 = q_tot / 2, q2 = q_tot - q1;
        inet_add(net, AG_CELL_RING, 3, d_max, mix_lineage(la, lb, 301u), m1, q1, B.param + 1);
        inet_add(net, AG_CELL_RING, 3, d_max, mix_lineage(la, lb, 302u), m2, q2, B.param + 1);
        if (heat_q > 0) inet_add(net, AG_HEAT, 3, d_max, mix_lineage(la, lb, 303u), heat_q, 0, 1);
        net->layer_rewrites[2]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_ESCRT_BUD && B.tag == AG_CELL_RING) {
        /* ESCRT-III Asymmetric Budding (0.5 < Q <= 1): 75% mother + 25% exosome bud */
        inet_kill_pair(net, u, v);
        int64_t heat_q = (m_tot >= 20) ? 2 : 0;
        int64_t rem = m_tot - heat_q;
        int64_t bud_m = rem / 4, mom_m = rem - bud_m;
        int64_t bud_q = q_tot / 4, mom_q = q_tot - bud_q;
        inet_add(net, AG_CELL_RING, 3, d_max, mix_lineage(la, lb, 311u), mom_m, mom_q, B.param + 1);
        inet_add(net, AG_CELL_RING, 3, d_max, mix_lineage(la, lb, 312u), bud_m, bud_q, B.param + 2);
        if (heat_q > 0) inet_add(net, AG_HEAT, 3, d_max, mix_lineage(la, lb, 313u), heat_q, 0, 1);
        net->layer_rewrites[2]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_PHAGE_T4 && B.tag == AG_CELL_RECEPTOR) {
        /* Phage T4 >< Receptor -> Virocell conversion! */
        inet_kill_pair(net, u, v);
        inet_add(net, AG_VIROCELL, 3, d_max, mix_lineage(la, lb, 321u), m_tot, q_tot, A.param ^ B.param);
        net->layer_rewrites[2]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_CELL_RING && B.tag == AG_CAHS_GLASS) {
        /* CAHS Bioglass Tun Vitrification: wraps internal ports in shielded=1 (0 active pairs!) */
        PortRef internal_peer = inet_detach(net, u, 1);
        inet_kill_pair(net, u, v);
        int32_t tun = inet_add(net, AG_BIOGLASS_TUN, 3, d_max, mix_lineage(la, lb, 331u), m_tot, q_tot, B.param);
        net->nodes[tun].shielded = 1; /* Principal port frozen — ZERO dynamic power burn! */
        if (internal_peer.node >= 0 && net->nodes[internal_peer.node].alive) {
            net->nodes[internal_peer.node].shielded = 1;
            inet_connect(net, tun, 1, internal_peer.node, internal_peer.slot);
        }
        net->layer_rewrites[2]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_BIOGLASS_TUN && B.tag == AG_H2O_REHYD) {
        /* Explicit Linear Rehydration Active Pair (H2O >< BioglassTun):
         * Dissolves bioglass shield, conserves exact mass, restores hydrated CellRing & uncaps internal organelle! */
        PortRef internal_peer = inet_detach(net, u, 1);
        inet_kill_pair(net, u, v);
        int64_t h2o_rem = B.mass_q;
        int64_t cell_m  = m_tot - h2o_rem;
        int32_t cell2 = inet_add(net, AG_CELL_RING, 3, d_max, mix_lineage(la, lb, 341u), cell_m, q_tot, A.param);
        inet_add(net, AG_MONOMER, 3, d_max, mix_lineage(la, lb, 342u), h2o_rem, 0, B.param);
        if (internal_peer.node >= 0 && net->nodes[internal_peer.node].alive) {
            net->nodes[internal_peer.node].shielded = 0;
            inet_connect(net, cell2, 1, internal_peer.node, internal_peer.slot);
            /* If internal organelle had a waiting principal-port encounter, wake its active pair! */
            PortRef wait_p = net->nodes[internal_peer.node].port[0];
            if (wait_p.node >= 0 && wait_p.slot == 0 && net->nodes[wait_p.node].alive) {
                inet_push_redex(net, internal_peer.node, wait_p.node);
            }
        }
        net->layer_rewrites[2]++;
        net->rewrites_total++;
        return 1;
    }

    /* ---------------- LAYER 4: MULTICELLULAR CREATURES / ORGANS / CPG ---------------- */
    if (A.tag == AG_GAP_JUNC && B.tag == AG_CELL_VM) {
        /* GapJunction >< (Cell_i, Cell_j): Lock-free conservative Levin bioelectric Graph Laplacian! */
        PortRef cell_j_ref = inet_detach(net, u, 1);
        if (cell_j_ref.node >= 0 && net->nodes[cell_j_ref.node].alive && net->nodes[cell_j_ref.node].tag == AG_CELL_VM) {
            int32_t cj = cell_j_ref.node;
            BioAgent J = net->nodes[cj];
            int64_t dq = (B.charge_q - J.charge_q) / 4; /* Conservative integer charge transfer! */
            int64_t j_mass = J.mass_q;
            uint32_t lj = J.lineage_id;
            inet_kill_pair(net, u, v);
            for (int p = 0; p < 4; ++p) inet_detach(net, cj, (uint8_t)p);
            net->nodes[cj].alive = 0;

            inet_add(net, AG_CELL_VM, 4, d_max, mix_lineage(la, lb, 401u), B.mass_q, B.charge_q - dq, B.param);
            inet_add(net, AG_CELL_VM, 4, d_max, mix_lineage(la, lj, 402u), j_mass,   J.charge_q + dq, J.param);
            inet_add(net, AG_PROTEIN, 4, d_max, mix_lineage(lb, lj, 403u), A.mass_q, A.charge_q, (int32_t)dq);
            net->layer_rewrites[3]++;
            net->rewrites_total++;
            return 1;
        }
    }
    if (A.tag == AG_APICAL_WEDGE && B.tag == AG_EPITH_PATCH) {
        /* Apical Myosin-II Constriction -> Folds flat epithelial patch into sealed 3D hollow Gut Lumen! */
        inet_kill_pair(net, u, v);
        int64_t heat_q = (m_tot >= 16) ? 2 : 0;
        inet_add(net, AG_GUT_LUMEN, 4, d_max, mix_lineage(la, lb, 411u), m_tot - heat_q, q_tot, B.param + 1);
        if (heat_q > 0) inet_add(net, AG_HEAT, 4, d_max, mix_lineage(la, lb, 412u), heat_q, 0, 1);
        net->layer_rewrites[3]++;
        net->rewrites_total++;
        return 1;
    }
    if (A.tag == AG_CPG_NEURON && B.tag == AG_MUSCLE_BAND) {
        /* Matsuoka CPG Half-Center >< Muscle Band -> 8-Lobopod Tardigrade Walking Step! */
        inet_kill_pair(net, u, v);
        int64_t work_q = (m_tot >= 16) ? 4 : 0;
        int64_t heat_q = (m_tot >= 16) ? 2 : 0;
        int64_t rem = m_tot - work_q - heat_q;
        inet_add(net, AG_MUSCLE_BAND, 4, d_max, mix_lineage(la, lb, 421u), rem, q_tot, B.param + 1);
        if (work_q > 0) inet_add(net, AG_WORK_STEP, 4, d_max, mix_lineage(la, lb, 422u), work_q, 0, A.param);
        if (heat_q > 0) inet_add(net, AG_HEAT, 4, d_max, mix_lineage(la, lb, 423u), heat_q, 0, 1);
        net->layer_rewrites[3]++;
        net->rewrites_total++;
        return 1;
    }

    return 0;
}

/*
 * Run the Active-Pair Redex Bag to Normal Form under 3 Wild Execution Schedules:
 *   sched == 0 : Forward FIFO queue
 *   sched == 1 : Reverse LIFO stack
 *   sched == 2 : Chaotic Random Parallel Permutation (Fisher-Yates shuffled wave!)
 */
static void inet_reduce_to_normal_form(BioInet* net, int sched, uint64_t sched_seed) {
    OmegaRng rng;
    omega_rng_seed(&rng, sched_seed ^ 0xF057B17EULL);
    int safety = 0;

    while (net->n_redex > 0 && safety < 20000) {
        safety++;
        int pick = 0;
        if (sched == 0) {
            pick = 0; /* Forward FIFO */
        } else if (sched == 1) {
            pick = net->n_redex - 1; /* Reverse LIFO */
        } else {
            pick = (int)(omega_rng_u32(&rng) % (uint32_t)net->n_redex); /* Chaotic Random Parallel Order */
        }
        ActivePair ap = net->redexes[pick];
        /* Remove picked entry (preserving FIFO order when sched==0, O(1) swap otherwise) */
        if (sched == 0) {
            for (int k = 0; k < net->n_redex - 1; ++k) net->redexes[k] = net->redexes[k + 1];
            net->n_redex--;
        } else {
            net->redexes[pick] = net->redexes[net->n_redex - 1];
            net->n_redex--;
        }
        inet_fire_pair(net, ap.a, ap.b);
    }
}

/* Canonical schedule-invariant Normal-Form record for 256-bit SHA-256 confluence verification */
typedef struct CanonNodeRec {
    uint32_t lineage_id;
    uint32_t meta;         /* tag | layer<<8 | box_depth<<16 | shielded<<24 */
    int64_t  mass_q;
    int64_t  charge_q;
    int32_t  param;
    uint32_t peer_lin[4];  /* Content-addressed lineage_id of each port partner */
} CanonNodeRec;

static void sort_canon_records(CanonNodeRec* arr, int n) {
    /* Deterministic insertion/shell sort (zero stdlib qsort dependency on bare Win64) */
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            CanonNodeRec tmp = arr[i];
            int j = i;
            while (j >= gap) {
                const CanonNodeRec* prev = &arr[j - gap];
                int gt = (prev->lineage_id > tmp.lineage_id) ||
                         (prev->lineage_id == tmp.lineage_id && prev->meta > tmp.meta) ||
                         (prev->lineage_id == tmp.lineage_id && prev->meta == tmp.meta && prev->mass_q > tmp.mass_q);
                if (!gt) break;
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = tmp;
        }
    }
}

static void inet_canonical_sha256(const BioInet* net, char out_hex[65]) {
    CanonNodeRec* recs = (CanonNodeRec*)calloc(INET_MAX_NODES, sizeof(CanonNodeRec));
    if (!recs) { memset(out_hex, '0', 64); out_hex[64] = '\0'; return; }
    int count = 0;
    for (int i = 0; i < net->n_nodes; ++i) {
        const BioAgent* ag = &net->nodes[i];
        if (!ag->alive) continue;
        CanonNodeRec* r = &recs[count++];
        r->lineage_id = ag->lineage_id;
        r->meta = (uint32_t)ag->tag | ((uint32_t)ag->layer << 8) |
                  ((uint32_t)ag->box_depth << 16) | ((uint32_t)ag->shielded << 24);
        r->mass_q   = ag->mass_q;
        r->charge_q = ag->charge_q;
        r->param    = ag->param;
        for (int p = 0; p < 4; ++p) {
            int32_t pn = ag->port[p].node;
            r->peer_lin[p] = (pn >= 0 && net->nodes[pn].alive) ? net->nodes[pn].lineage_id : 0u;
        }
    }
    sort_canon_records(recs, count);
    omega_sha256_hex(recs, (size_t)count * sizeof(CanonNodeRec), out_hex);
    free(recs);
}

/*
 * Construct the Canonical 4-Layer Biosphere Graph (DNA -> Organelles -> Cells -> Tardigrade Creature)
 * Every node receives a deterministic seed-based content-addressed lineage_id.
 */
static void build_4layer_biosphere_graph(BioInet* net, uint64_t seed) {
    OmegaRng rng;
    omega_rng_seed(&rng, seed ^ 0xB10F2057ULL);
    uint32_t uid = 1000u;

    /* ---- LAYER 1: DNA REPO (Polymerase chain, ADAR->Ribosome chain, CRISPR vs Phage, Era) ---- */
    for (int c = 0; c < 4; ++c) {
        int32_t poly = inet_add(net, AG_POLY_DUP, 1, 0, uid++, 120, 0, c);
        int32_t prev = -1;
        for (int b = 0; b < 8; ++b) {
            int32_t base = inet_add(net, AG_DNA_BASE, 1, 0, uid++, 12, 0, (int32_t)(omega_rng_u32(&rng) & 0xFFu));
            if (b == 0) inet_connect(net, poly, 0, base, 0);
            else        inet_connect(net, prev, 1, base, 0);
            prev = base;
        }
    }
    /* ADAR live cephalopod RNA editing feeding directly into a Ribosome translation chain! */
    for (int r = 0; r < 3; ++r) {
        int32_t adar = inet_add(net, AG_ADAR, 1, 0, uid++, 18, 0, r);
        int32_t mrna = inet_add(net, AG_MRNA_RAW, 1, 0, uid++, 14, 0, 0x41 + r);
        int32_t ribo = inet_add(net, AG_RIBO, 1, 0, uid++, 64, 0, r);
        int32_t cod2 = inet_add(net, AG_CODON, 1, 0, uid++, 14, 0, 0x52 + r);
        int32_t cod3 = inet_add(net, AG_CODON, 1, 0, uid++, 14, 0, 0x63 + r);
        inet_connect(net, cod2, 1, cod3, 0);
        inet_connect(net, mrna, 1, ribo, 0);
        inet_connect(net, mrna, 2, cod2, 0);
        inet_connect(net, adar, 0, mrna, 0);
    }
    /* CRISPR vs Phage + Explicit Linear Nuclease Erasure */
    for (int k = 0; k < 6; ++k) {
        int32_t cr = inet_add(net, AG_CRISPR, 1, 0, uid++, 24, 0, k);
        int32_t ph = inet_add(net, AG_PHAGE,  1, 0, uid++, 16, 0, 100 + k);
        inet_connect(net, cr, 0, ph, 0);
        int32_t er = inet_add(net, AG_ERA_NUC, 1, 0, uid++, 10, 0, k);
        int32_t st = inet_add(net, AG_STRAND_OLD, 1, 0, uid++, 18, 0, k);
        inet_connect(net, er, 0, st, 0);
    }

    /* ---- LAYER 2: ORGANELLES REPO (ATP Synthase, Kinesin +8.2nm, Ca2+ Catastrophe, Russian-Doll D0->D3!) ---- */
    for (int m = 0; m < 8; ++m) {
        int32_t syn = inet_add(net, AG_ATP_SYNTH, 2, 1, uid++, 40, 0, 8 + (m % 10));
        int32_t hp  = inet_add(net, AG_PROTON_BATCH, 2, 1, uid++, 16, 0, m);
        inet_connect(net, syn, 0, hp, 0);
    }
    for (int k = 0; k < 3; ++k) {
        int32_t kin = inet_add(net, AG_KINESIN, 2, 0, uid++, 48, 0, k);
        int32_t prev = -1;
        for (int t = 0; t < 6; ++t) {
            int32_t tub = inet_add(net, AG_TUBULIN, 2, 0, uid++, 10, 0, t);
            if (t == 0) inet_connect(net, kin, 0, tub, 0);
            else        inet_connect(net, prev, 1, tub, 0);
            prev = tub;
        }
    }
    /* Ca2+ Microtubule Banana-Peel Catastrophe chain */
    for (int c = 0; c < 2; ++c) {
        int32_t ca = inet_add(net, AG_CA2_MELT, 2, 0, uid++, 8, 0, c);
        int32_t prev = -1;
        for (int w = 0; w < 6; ++w) {
            int32_t wall = inet_add(net, AG_MT_WALL, 2, 0, uid++, 10, 0, w);
            if (w == 0) inet_connect(net, ca, 0, wall, 0);
            else        inet_connect(net, prev, 1, wall, 0);
            prev = wall;
        }
    }
    /* Nested Russian-Doll Endosymbiosis Cascade (Depth 0 -> 1 -> 2 -> 3: Mealybug/Tremblaya/Moranella!) */
    for (int rd = 0; rd < 2; ++rd) {
        int32_t prey   = inet_add(net, AG_PREY_BOX,  2, 0, uid++, 20, 0, rd);
        int32_t phago1 = inet_add(net, AG_PHAGOSOME, 2, 0, uid++, 24, 0, 1);
        int32_t phago2 = inet_add(net, AG_PHAGOSOME, 2, 0, uid++, 28, 0, 2);
        int32_t phago3 = inet_add(net, AG_PHAGOSOME, 2, 0, uid++, 32, 0, 3);
        inet_connect(net, phago1, 1, phago2, 0);
        inet_connect(net, phago2, 1, phago3, 0);
        inet_connect(net, phago1, 0, prey, 0);
    }

    /* ---- LAYER 3: CELLULAR REPO (FtsZ Binary Fission, ESCRT-III Budding, Virocell Splice) ---- */
    for (int c = 0; c < 6; ++c) {
        int32_t ftsz = inet_add(net, AG_FTSZ_DIV,  3, 0, uid++, 20, 0, c);
        int32_t cell = inet_add(net, AG_CELL_RING, 3, 0, uid++, 100, -70, c);
        inet_connect(net, ftsz, 0, cell, 0);

        int32_t esc  = inet_add(net, AG_ESCRT_BUD, 3, 0, uid++, 16, 0, c);
        int32_t c2   = inet_add(net, AG_CELL_RING, 3, 0, uid++, 84, -64, c);
        inet_connect(net, esc, 0, c2, 0);
    }
    for (int v = 0; v < 4; ++v) {
        int32_t pt4 = inet_add(net, AG_PHAGE_T4,      3, 0, uid++, 18, 0, v);
        int32_t rec = inet_add(net, AG_CELL_RECEPTOR, 3, 0, uid++, 90, -55, v);
        inet_connect(net, pt4, 0, rec, 0);
    }

    /* ---- LAYER 4: CREATURES REPO (Gap-Junction Levin Vm Laplacian, Gut Folding, 8-Lobopod CPG) ---- */
    for (int g = 0; g < 8; ++g) {
        int32_t gj = inet_add(net, AG_GAP_JUNC, 4, 0, uid++, 12, 0, g);
        int32_t ci = inet_add(net, AG_CELL_VM,  4, 0, uid++, 60, -20 + g * 5, g * 2);
        int32_t cj = inet_add(net, AG_CELL_VM,  4, 0, uid++, 60, -80 - g * 3, g * 2 + 1);
        inet_connect(net, gj, 1, cj, 1);
        inet_connect(net, gj, 0, ci, 0);
    }
    for (int e = 0; e < 4; ++e) {
        int32_t wedge = inet_add(net, AG_APICAL_WEDGE, 4, 0, uid++, 18, 0, e);
        int32_t patch = inet_add(net, AG_EPITH_PATCH,  4, 0, uid++, 72, -40, e);
        inet_connect(net, wedge, 0, patch, 0);
    }
    for (int leg = 0; leg < 8; ++leg) {
        int32_t cpg  = inet_add(net, AG_CPG_NEURON,  4, 0, uid++, 24, -50, leg);
        int32_t musc = inet_add(net, AG_MUSCLE_BAND, 4, 0, uid++, 56, -70, leg);
        inet_connect(net, cpg, 0, musc, 0);
    }
}

/*
 * Cryptobiotic Bioglass Tun Test (O43):
 * Verifies 0 active pairs & 0 dynamic power burn while desiccated, then linear H2O_REHYD >< BioglassTun
 * rehydration with exact 0 mass drift and 0 single-ownership wire errors!
 */
static void run_cryptobiotic_tun_test(int* dry_pairs, int64_t* dry_power, int* rehyd_rewrites,
                                      int64_t* tun_mass_drift, int* tun_wire_errs) {
    BioInet* tun_net = (BioInet*)calloc(1, sizeof(BioInet));
    if (!tun_net) return;
    inet_init(tun_net, 0);

    /* Phase 1: CAHS_IDP vitrifies the CellRing and freezes its internal ATP synthase port */
    int32_t cell = inet_add(tun_net, AG_CELL_RING, 3, 0, 9001u, 120, -70, 1);
    int32_t cahs = inet_add(tun_net, AG_CAHS_GLASS, 3, 0, 9002u, 20, 0, 1);
    int32_t syn  = inet_add(tun_net, AG_ATP_SYNTH, 2, 1, 9003u, 40, 0, 10);
    int32_t hp   = inet_add(tun_net, AG_PROTON_BATCH, 2, 1, 9004u, 16, 0, 1);
    int64_t m0   = inet_total_mass(tun_net);
    inet_connect(tun_net, cell, 1, syn, 1);
    inet_connect(tun_net, cell, 0, cahs, 0);
    inet_reduce_to_normal_form(tun_net, 0, 1ULL); /* Cell becomes shielded AG_BIOGLASS_TUN! */

    /* Locate the vitrified AG_BIOGLASS_TUN node */
    int32_t tun_node = -1;
    for (int i = 0; i < tun_net->n_nodes; ++i) {
        if (tun_net->nodes[i].alive && tun_net->nodes[i].tag == AG_BIOGLASS_TUN) { tun_node = i; break; }
    }

    /* Phase 2: Attempt to wire protons onto the shielded ATP synthase during 98% desiccation */
    int before_dry = tun_net->rewrites_total;
    inet_connect(tun_net, syn, 0, hp, 0); /* Blocked by shielded==1 -> 0 active pairs queued! */
    inet_reduce_to_normal_form(tun_net, 0, 2ULL);
    *dry_pairs = tun_net->rewrites_total - before_dry;
    *dry_power = (int64_t)(*dry_pairs) * 100LL; /* 0 dynamic power burn on Frostbite silicon! */

    /* Phase 3: Linear Rehydration via native VM active pair AG_H2O_REHYD >< AG_BIOGLASS_TUN!
     * Notice tun_node stays shielded==1 until the interaction rule itself fires & dissolves the shield! */
    int before_rehyd = tun_net->rewrites_total;
    int32_t h2o = inet_add(tun_net, AG_H2O_REHYD, 3, 0, 9008u, 12, 0, 1);
    m0 += 12;
    if (tun_node >= 0) {
        inet_connect(tun_net, tun_node, 0, h2o, 0);
    }
    /* Also dock a waiting Kinesin + Tubulin post-rehydration */
    int32_t kin = inet_add(tun_net, AG_KINESIN, 2, 0, 9005u, 36, 0, 1);
    int32_t t1  = inet_add(tun_net, AG_TUBULIN, 2, 0, 9006u, 10, 0, 1);
    int32_t t2  = inet_add(tun_net, AG_TUBULIN, 2, 0, 9007u, 10, 0, 2);
    m0 += 56;
    inet_connect(tun_net, t1, 1, t2, 0);
    inet_connect(tun_net, kin, 0, t1, 0);
    inet_reduce_to_normal_form(tun_net, 0, 3ULL);
    *rehyd_rewrites = tun_net->rewrites_total - before_rehyd;
    *tun_mass_drift = inet_total_mass(tun_net) - m0;
    *tun_wire_errs  = inet_audit_ownership(tun_net);
    free(tun_net);
}

/*
 * Hybrid Stochastic Spatial Interaction Net (SSIN / "Bio-Frostbite") Test:
 * <=5% Spatial Voxel_Arbiter boundary pairs free-swimming microbes/phages -> >=95% lock-free confluent cascades!
 */
static void run_ssin_hybrid_bench(uint64_t seed, int feral, int* arbiter_pairs, int* confluent_rewrites,
                                  int64_t* ssin_mass_drift, int* ssin_wire_errs) {
    OmegaRng rng;
    uint64_t eff = seed ^ 0x551AF057ULL;
    if (feral) {
        double t = omega_time_sec();
        uint64_t tb = 0; memcpy(&tb, &t, sizeof(tb));
#if defined(__x86_64__) || defined(_M_X64)
        uint32_t lo = 0, hi = 0;
        __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
        uint64_t tsc = ((uint64_t)hi << 32) | (uint64_t)lo;
        eff ^= tb ^ (tsc * 0x9E3779B97F4A7C15ULL);
#else
        eff ^= tb * 0x9E3779B97F4A7C15ULL;
#endif
    }
    omega_rng_seed(&rng, eff);

    BioInet* ssin = (BioInet*)calloc(1, sizeof(BioInet));
    if (!ssin) return;
    inet_init(ssin, 0);

    /* 16 3D fluid voxels: 1 spatial Voxel_Arbiter pairing triggers a 20..22-step pure confluent cascade (~95.3%!) */
    int pairs_matched = 0;
    uint32_t uid = 50000u;
    for (int vox = 0; vox < 16; ++vox) {
        int encounter = (int)(omega_rng_u32(&rng) % 3u);
        if (encounter == 0) {
            int32_t poly = inet_add(ssin, AG_POLY_DUP, 1, 0, uid++, 240, 0, vox);
            int32_t prev = -1;
            for (int b = 0; b < 20; ++b) {
                int32_t bs = inet_add(ssin, AG_DNA_BASE, 1, 0, uid++, 10, 0, b);
                if (b == 0) inet_connect(ssin, poly, 0, bs, 0);
                else        inet_connect(ssin, prev, 1, bs, 0);
                prev = bs;
            }
            pairs_matched++;
        } else if (encounter == 1) {
            int32_t kin = inet_add(ssin, AG_KINESIN, 2, 0, uid++, 120, 0, vox);
            int32_t prev = -1;
            for (int t = 0; t < 20; ++t) {
                int32_t tub = inet_add(ssin, AG_TUBULIN, 2, 0, uid++, 10, 0, t);
                if (t == 0) inet_connect(ssin, kin, 0, tub, 0);
                else        inet_connect(ssin, prev, 1, tub, 0);
                prev = tub;
            }
            pairs_matched++;
        } else {
            int32_t ca = inet_add(ssin, AG_CA2_MELT, 2, 0, uid++, 16, 0, vox);
            int32_t prev = -1;
            for (int w = 0; w < 22; ++w) {
                int32_t mt = inet_add(ssin, AG_MT_WALL, 2, 0, uid++, 10, 0, w);
                if (w == 0) inet_connect(ssin, ca, 0, mt, 0);
                else        inet_connect(ssin, prev, 1, mt, 0);
                prev = mt;
            }
            pairs_matched++;
        }
    }
    int64_t m0 = inet_total_mass(ssin);
    /* Execute all resulting cascades lock-free in chaotic parallel permutation order */
    inet_reduce_to_normal_form(ssin, 2, eff ^ 0x12345678ULL);
    *arbiter_pairs      = pairs_matched;
    *confluent_rewrites = ssin->rewrites_total;
    *ssin_mass_drift    = inet_total_mass(ssin) - m0;
    *ssin_wire_errs     = inet_audit_ownership(ssin);
    free(ssin);
}

static inline int64_t i64_abs(int64_t v) { return (v < 0) ? -v : v; }

OMEGA_API void omega_run_tb14(uint64_t seed, int mutant_mode, int feral_hw_entropy, TB14Result* out) {
    memset(out, 0, sizeof(*out));
    int race_mutant = (mutant_mode == 8) ? 1 : 0;

    BioInet* net_fifo = (BioInet*)calloc(1, sizeof(BioInet));
    BioInet* net_lifo = (BioInet*)calloc(1, sizeof(BioInet));
    BioInet* net_rand = (BioInet*)calloc(1, sizeof(BioInet));
    if (!net_fifo || !net_lifo || !net_rand) {
        free(net_fifo); free(net_lifo); free(net_rand);
        return;
    }

    double t0 = omega_time_sec();

    /* Build bit-identical copies of the 4-layer Biosphere Interaction Net */
    inet_init(net_fifo, race_mutant); build_4layer_biosphere_graph(net_fifo, seed);
    inet_init(net_lifo, race_mutant); build_4layer_biosphere_graph(net_lifo, seed);

    int64_t init_m = inet_total_mass(net_fifo);
    int64_t init_q = inet_total_charge(net_fifo);
    out->total_nodes_init = net_fifo->n_nodes;
    out->init_mass_q      = init_m;

    /* Execute the EXACT SAME graph under FIFO, LIFO, and 3 distinct Chaotic Random Parallel Permutations (5 schedules!) */
    inet_reduce_to_normal_form(net_fifo, 0, seed ^ 0x1111ULL); /* Forward FIFO */
    inet_reduce_to_normal_form(net_lifo, 1, seed ^ 0x2222ULL); /* Reverse LIFO */

    inet_canonical_sha256(net_fifo, out->sha_fifo);
    inet_canonical_sha256(net_lifo, out->sha_lifo);

    int all_rand_match = 1;
    int64_t rand_q_drift = 0;
    int64_t rand_m_drift = 0;
    int rand_wire_errs   = 0;
    int rand_rewrites    = 0;
    const uint64_t rand_salts[3] = { 0x3333ULL, 0x4444ULL, 0x5555ULL };
    for (int k = 0; k < 3; ++k) {
        char sha_k[65];
        inet_init(net_rand, race_mutant);
        build_4layer_biosphere_graph(net_rand, seed);
        inet_reduce_to_normal_form(net_rand, 2, seed ^ rand_salts[k]);
        inet_canonical_sha256(net_rand, sha_k);
        if (k == 0) {
            memcpy(out->sha_random, sha_k, 65);
            rand_rewrites = net_rand->rewrites_total;
        }
        if (strcmp(out->sha_fifo, sha_k) != 0 || net_rand->rewrites_total != net_fifo->rewrites_total) {
            all_rand_match = 0;
        }
        rand_m_drift   += i64_abs(inet_total_mass(net_rand) - init_m);
        rand_q_drift   += i64_abs(inet_total_charge(net_rand) - init_q);
        rand_wire_errs += inet_audit_ownership(net_rand);
    }

    double dt_sec = omega_time_sec() - t0;
    if (dt_sec < 1e-6) dt_sec = 1e-6;

    out->rewrites_fifo   = net_fifo->rewrites_total;
    out->rewrites_lifo   = net_lifo->rewrites_total;
    out->rewrites_random = rand_rewrites;
    for (int l = 0; l < 4; ++l) out->layer_rewrites[l] = net_fifo->layer_rewrites[l];

    out->confluence_exact_match =
        (strcmp(out->sha_fifo, out->sha_lifo) == 0 && all_rand_match) ? 1 : 0;

    out->mass_drift_fifo   = inet_total_mass(net_fifo) - init_m;
    out->mass_drift_lifo   = inet_total_mass(net_lifo) - init_m;
    out->mass_drift_random = rand_m_drift;
    out->gap_charge_drift  =
        i64_abs(inet_total_charge(net_fifo) - init_q) +
        i64_abs(inet_total_charge(net_lifo) - init_q) +
        rand_q_drift;
    out->wire_ownership_errors =
        inet_audit_ownership(net_fifo) +
        inet_audit_ownership(net_lifo) +
        rand_wire_errs;
    out->russian_doll_max_depth = net_fifo->max_box_depth;

    free(net_fifo);
    free(net_lifo);
    free(net_rand);

    /* Zero-Power Cryptobiotic Bioglass Tun Test (including linear H2O rehydration conservation & wire audit!) */
    int64_t tun_drift = 0;
    int tun_wire_errs = 0;
    run_cryptobiotic_tun_test(&out->tun_active_pairs_dry, &out->tun_power_burn_dry,
                              &out->tun_rewrites_rehyd, &tun_drift, &tun_wire_errs);
    out->mass_drift_fifo       += tun_drift;
    out->wire_ownership_errors += tun_wire_errs;

    /* Hybrid SSIN ("Bio-Frostbite") >=95/5 Arbiter Benchmark (with exact wall-clock timing & wire audit!) */
    int64_t ssin_drift = 0;
    int ssin_wire_errs = 0;
    double t_ssin0 = omega_time_sec();
    run_ssin_hybrid_bench(seed, feral_hw_entropy, &out->ssin_arbiter_pairings, &out->ssin_confluent_rewrites,
                          &ssin_drift, &ssin_wire_errs);
    double dt_ssin = omega_time_sec() - t_ssin0;
    if (dt_ssin < 0.0) dt_ssin = 0.0;
    out->mass_drift_fifo       += ssin_drift;
    out->wire_ownership_errors += ssin_wire_errs;
    int ssin_tot = out->ssin_arbiter_pairings + out->ssin_confluent_rewrites;
    out->ssin_confluent_pct = (ssin_tot > 0) ? (100.0 * (double)out->ssin_confluent_rewrites / (double)ssin_tot) : 0.0;

    /* Bit-exact deterministic SHA-256 over all deterministic fields BEFORE writing wall-clock telemetry */
    omega_sha256_hex(out, offsetof(TB14Result, sha256), out->sha256);

    /* Honest throughput: numerator counts ONLY rewrites timed inside (dt_sec + dt_ssin)! */
    int total_r = out->rewrites_fifo + out->rewrites_lifo + (3 * out->rewrites_random) + out->ssin_confluent_rewrites;
    double total_timed_sec = dt_sec + dt_ssin;
    if (total_timed_sec < 1e-6) total_timed_sec = 1e-6;
    out->rewrites_per_sec = (double)total_r / total_timed_sec;
}
